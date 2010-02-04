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

HardCamera::HardCamera(const scene::Hardware& config,
                       int iplDepth, int nChannels)

  :framePixels(config.resolution.at(0) * config.resolution.at(1))
  ,frameSize(cvSize(config.resolution.at(0), config.resolution.at(1)))
  ,nChannels(nChannels)
  ,iplDepth(iplDepth)
  ,distortedFrame()
  ,undistortedFrame()
  ,cameraIdentifier(config.identifier)
  ,framesAccessMutex()
  ,mapx(this->frameSize, IPL_DEPTH_32F, 1)
  ,mapy(this->frameSize, IPL_DEPTH_32F, 1)
  ,showImages(global_options.showImages)
  ,windowName("Camera_" + cameraIdentifier)
  ,saveDistortedImages(global_options.saveDistortedImages &&
                       !config.saveImageFormat.empty())
  ,saveUndistortedImages(global_options.saveUndistortedImages &&
                         !config.saveImageFormat.empty())
  ,saveImageFormat(config.saveImageFormat)
  ,imagesCounter(0)
{
  undistortedFrameBuffer[0].reset(new IplImageWrapper(this->frameSize,
                                                      this->iplDepth,
                                                      this->nChannels));
  undistortedFrameBuffer[1].reset(new IplImageWrapper(this->frameSize,
                                                      this->iplDepth,
                                                      this->nChannels));

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
  if (this->showImages)
  {
    cvDestroyWindow(this->windowName.c_str());
  }
}

IplImageWrapper::IplParameters
HardCamera::getImageParameters() const
{
  return IplImageWrapper::IplParameters(this->frameSize, this->iplDepth,
                                        this->nChannels);
}

void
HardCamera::updateCurrentFrame(boost::shared_ptr<IplImageWrapper> frame)
{
  static int undistortedIdx = 0;

  cvRemap(*frame, *this->undistortedFrameBuffer[undistortedIdx],
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
    cvSaveImage(this->saveImageFormat.str().c_str(), *this->distortedFrame);
  }

  if (this->saveUndistortedImages)
  {
    this->saveImageFormat % this->cameraIdentifier % this->imagesCounter
                          % "u";
    cvSaveImage(this->saveImageFormat.str().c_str(), *this->undistortedFrame);
  }

  if (this->showImages)
  {
    cvShowImage(this->windowName.c_str(), *this->undistortedFrame);
    cvWaitKey(5);
  }

  undistortedIdx = (undistortedIdx+1) % 2;
  this->imagesCounter++;
}

void
HardCamera::getDistortedFrame(IplImageWrapper& image)
{
  boost::shared_lock<boost::shared_mutex> lock(this->framesAccessMutex);
  image = *this->distortedFrame;
}

void
HardCamera::getUndistortedFrame(IplImageWrapper& image)
{
  boost::shared_lock<boost::shared_mutex> lock(this->framesAccessMutex);
  image = *this->undistortedFrame;
}
