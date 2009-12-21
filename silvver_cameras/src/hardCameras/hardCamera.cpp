/* Copyright 2009 Renato Florentino Garcia <fgar.renato@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "hardCamera.hpp"

#include <boost/filesystem.hpp>
#include <stdint.h>

#include <opencv/highgui.h>

#include "../globalOptions.hpp"

namespace bfs = boost::filesystem;

extern globalOptions::Options global_options;

HardCamera::HardCamera(const scene::Hardware& config, unsigned bitsPerPixel)
  :frameSize(config.resolution.at(0) * config.resolution.at(1))
  ,frameWidth(config.resolution.at(0))
  ,frameHeight(config.resolution.at(1))
  ,bitsPerPixel(bitsPerPixel)
  ,distortedFrame(NULL)
  ,undistortedFrame(NULL)
  ,cameraIdentifier(config.identifier)
  ,framesAccessMutex()
  ,mapx(cvCreateImage(cvSize(this->frameWidth, this->frameHeight),
                      IPL_DEPTH_32F, 1))
  ,mapy(cvCreateImage(cvSize(this->frameWidth, this->frameHeight),
                      IPL_DEPTH_32F, 1))
  ,showImages(global_options.showImages)
  ,windowName("Camera_" + cameraIdentifier)
  ,saveDistortedImages(global_options.saveDistortedImages &&
                       !config.saveImageFormat.empty())
  ,saveUndistortedImages(global_options.saveUndistortedImages &&
                         !config.saveImageFormat.empty())
  ,saveImageFormat(config.saveImageFormat)
  ,imagesCounter(0)
{
  createIplImage(&this->undistortedFrameBuffer[0]);
  createIplImage(&this->undistortedFrameBuffer[1]);

  CvMat* intrinsic = cvCreateMat(3, 3, CV_32FC1);
  CvMat* distortion = cvCreateMat(5, 1, CV_32FC1);

  CV_MAT_ELEM(*intrinsic, float, 0, 0) = config.focalLength.at(0);
  CV_MAT_ELEM(*intrinsic, float, 0, 1) = 0;
  CV_MAT_ELEM(*intrinsic, float, 0, 2) = config.principalPoint.at(0);
  CV_MAT_ELEM(*intrinsic, float, 1, 0) = 0;
  CV_MAT_ELEM(*intrinsic, float, 1, 1) = config.focalLength.at(1);
  CV_MAT_ELEM(*intrinsic, float, 1, 2) = config.principalPoint.at(1);
  CV_MAT_ELEM(*intrinsic, float, 2, 0) = 0;
  CV_MAT_ELEM(*intrinsic, float, 2, 1) = 0;
  CV_MAT_ELEM(*intrinsic, float, 2, 2) = 1;

  CV_MAT_ELEM(*distortion, float, 0, 0) = config.radialCoef.at(0);
  CV_MAT_ELEM(*distortion, float, 1, 0) = config.radialCoef.at(1);
  CV_MAT_ELEM(*distortion, float, 2, 0) = config.tangentialCoef.at(0);
  CV_MAT_ELEM(*distortion, float, 3, 0) = config.tangentialCoef.at(1);
  CV_MAT_ELEM(*distortion, float, 4, 0) = config.radialCoef.at(2);

  cvInitUndistortMap(intrinsic, distortion, this->mapx, this->mapy);

  cvRelease((void**)&intrinsic);
  cvRelease((void**)&distortion);

  if (this->showImages)
  {
    cvNamedWindow(this->windowName.c_str());
  }

  // Ignore if using not all placeholders in format string.
  this->saveImageFormat.exceptions(boost::io::all_error_bits ^
                                   boost::io::too_many_args_bit);
  if (this->saveUndistortedImages || this->saveDistortedImages)
  {
    this->saveImageFormat % this->cameraIdentifier % this->imagesCounter
                          % "u";
    // Create directory where images will be saved if it don't exists yet.
    bfs::path path(this->saveImageFormat.str());
    path = path.remove_filename();
    bfs::create_directories(path);
  }
}

HardCamera::~HardCamera()
{
  cvReleaseImage(&this->mapx);
  cvReleaseImage(&this->mapy);

  for (int i = 0; i < 2; ++i)
  {
    cvReleaseImage(&this->undistortedFrameBuffer[i]);
  }

  if (this->showImages)
  {
    cvDestroyWindow(this->windowName.c_str());
  }
}

void
HardCamera::createIplImage(IplImage** iplImage) const
{
  int depth;
  if (this->bitsPerPixel == 8)
  {
    depth = IPL_DEPTH_8U;
  }
  else if (this->bitsPerPixel == 16)
  {
    depth = IPL_DEPTH_16U;
  }
  else
  {
    throw camera_parameter_error("Invalid image depth");
  }

  *iplImage = cvCreateImage(cvSize(this->frameWidth, this->frameHeight),
                            depth, 3);
}

template <class T>
void
HardCamera::rgb2bgr(const T* const input, T* const output,
                    const unsigned nPixels)
{
  for (int i=0; i<nPixels*3; i+=3)
  {
    output[i]   = input[i+2];
    output[i+1] = input[i+1];
    output[i+2] = input[i];
  }
}

void
HardCamera::fixChannelOrder(const IplImage* const input,
                            IplImage* const output)
{
  if (input->depth != output->depth ||
      input->width != output->width ||
      input->height != output->height)
  {
    // throw BIZIU
  }

  unsigned nPixels = input->width*input->height;
  if (input->depth == IPL_DEPTH_8U ||
      input->depth == IPL_DEPTH_8S)
  {
    this->rgb2bgr(reinterpret_cast<const uint8_t* const>(input->imageData),
                  reinterpret_cast<uint8_t* const>(output->imageData),
                  nPixels);
  }
  else if (input->depth == IPL_DEPTH_16S)
  {
    this->rgb2bgr(reinterpret_cast<const uint16_t* const>(input->imageData),
                  reinterpret_cast<uint16_t* const>(output->imageData),
                  nPixels);

  }
  else if (input->depth == IPL_DEPTH_32S ||
           input->depth == IPL_DEPTH_32F)
  {
    this->rgb2bgr(reinterpret_cast<const uint32_t* const>(input->imageData),
                  reinterpret_cast<uint32_t* const>(output->imageData),
                  nPixels);

  }
  // else if (input->depth == IPL_DEPTH_64F)
  // {
  // }
  else
  {
    // throw BIZIU
  }

  return;
}

void
HardCamera::updateCurrentFrame(IplImage* frame)
{
  static int undistortedIdx = 0;

  cvRemap(frame, this->undistortedFrameBuffer[undistortedIdx],
          this->mapx, this->mapy,
          CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(0));

  this->framesAccessMutex.lock();
  this->distortedFrame = frame;
  this->undistortedFrame = this->undistortedFrameBuffer[undistortedIdx];
  this->framesAccessMutex.unlock();

  // Notify the classes observing this camera.
  notify();

  if (this->saveDistortedImages)
  {
    this->saveImageFormat % this->cameraIdentifier % this->imagesCounter
                          % "d";
    cvSaveImage(this->saveImageFormat.str().c_str(), this->distortedFrame);
  }

  if (this->saveUndistortedImages)
  {
    this->saveImageFormat % this->cameraIdentifier % this->imagesCounter
                          % "u";
    cvSaveImage(this->saveImageFormat.str().c_str(), this->undistortedFrame);
  }

  if (this->showImages)
  {
    cvShowImage(this->windowName.c_str(), this->undistortedFrame);
    cvWaitKey(5);
  }

  undistortedIdx = (undistortedIdx+1) % 2;
  this->imagesCounter++;
}

void
HardCamera::getDistortedFrame(IplImage** image)
{
  boost::shared_lock<boost::shared_mutex> lock(this->framesAccessMutex);
  *image = cvCloneImage(this->distortedFrame);
}

void
HardCamera::getUndistortedFrame(IplImage** image)
{
  boost::shared_lock<boost::shared_mutex> lock(this->framesAccessMutex);
  *image = cvCloneImage(this->undistortedFrame);
}
