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

#include <opencv/highgui.h>

#include "../globalOptions.hpp"

extern globalOptions::Options global_options;

HardCamera::HardCamera(const scene::Hardware& config, unsigned bitsPerPixel,
                       std::string cameraIdenfier)
  :frameSize(config.resolution.at(0) * config.resolution.at(1))
  ,frameWidth(config.resolution.at(0))
  ,frameHeight(config.resolution.at(1))
  ,bitsPerPixel(bitsPerPixel)
  ,unreadImage()
  ,cameraIdenfier(cameraIdenfier)
  ,mapx(cvCreateImage(cvSize(this->frameWidth, this->frameHeight),
                      IPL_DEPTH_32F, 1))
  ,mapy(cvCreateImage(cvSize(this->frameWidth, this->frameHeight),
                      IPL_DEPTH_32F, 1))
  ,showImages(global_options.showImages)
  ,windowName("Camera")
  ,saveImages(global_options.saveImages)
  ,saveImageformat(config.saveImageFormat)
  ,savedImagesCounter(0)
{
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
}

HardCamera::~HardCamera()
{
  cvReleaseImage(&(this->mapx));
  cvReleaseImage(&(this->mapy));

  if (this->showImages)
  {
    cvDestroyWindow(this->windowName.c_str());
  }
}

unsigned
HardCamera::addClient()
{
  this->unreadImage.push_back(false);
  return (this->unreadImage.size() - 1);
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

void
HardCamera::undistortFrame(IplImage* frame)
{
  IplImage *tmp = cvCloneImage(frame);

  cvRemap(tmp, frame, this->mapx, this->mapy,
          CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(0));

  cvReleaseImage(&tmp);
}

void
HardCamera::captureRectFrame(IplImage** image, unsigned clientUid)
{
  this->captureFrame(image, clientUid);
  undistortFrame(*image);

  if (this->showImages)
  {
    cvShowImage(this->windowName.c_str(), *image);
    cvWaitKey(5);
  }
  if (this->saveImages)
  {
    this->saveImageformat % this->cameraIdenfier % this->savedImagesCounter;
    cvSaveImage(this->saveImageformat.str().c_str(), *image);
    this->savedImagesCounter++;
  }
}
