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
#include <cstddef>
#include <stdint.h>

#include <opencv/highgui.h>

#include "../globalOptions.hpp"
#include "../exceptions.hpp"

namespace bfs = boost::filesystem;

extern globalOptions::Options global_options;

HardCamera::HardCamera(const scene::HardCamera& config, int iplDepth)
  :hardCameraUid(config.hardCameraUid)
  ,framePixels(config.resolution.at(0) * config.resolution.at(1))
  ,frameSize(cvSize(config.resolution.at(0), config.resolution.at(1)))
  ,iplDepth(iplDepth)
  ,warpedFrame(NULL)
  ,unwarpedFrame(NULL)
  ,unwarpedFrameBuffer()
  ,framesAccessMutex()
  ,mapx(this->frameSize, IPL_DEPTH_32F, 1)
  ,mapy(this->frameSize, IPL_DEPTH_32F, 1)
  ,showImages(global_options.showImages)
  ,windowName("Camera_" + config.hardCameraUid)
  ,saveWarpedImages(global_options.saveWarpedImages &&
                    !config.saveImageFormat.empty())
  ,saveUnwarpedImages(global_options.saveUnwarpedImages &&
                      !config.saveImageFormat.empty())
  ,saveImageFormat(HardCamera::createFormat(config.saveImageFormat,
                                            config.hardCameraUid))
  ,imagesCounter(0)
  ,saveTimestamp(global_options.saveTimestamp)
  ,timestampWriter()
{
  unwarpedFrameBuffer[0].image = IplImageWrapper(this->frameSize,
                                                 this->iplDepth, 3);
  unwarpedFrameBuffer[1].image = IplImageWrapper(this->frameSize,
                                                 this->iplDepth, 3);

  CvMat* intrinsic = cvCreateMat(3, 3, CV_32FC1);
  // Opencv 1.0.0 can handle only 4x1 distortion matrix
  // CvMat* distortion = cvCreateMat(5, 1, CV_32FC1);
  CvMat* distortion = cvCreateMat(4, 1, CV_32FC1);

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
  // CV_MAT_ELEM(*distortion, float, 4, 0) = config.radialCoef.at(2);

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
  if (this->saveUnwarpedImages || this->saveWarpedImages)
  {
    this->saveImageFormat % this->hardCameraUid % this->imagesCounter
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
  if (this->saveTimestamp)
  {
    this->saveImageFormat % this->hardCameraUid % this->imagesCounter
                          % "u";
    bfs::path path(this->saveImageFormat.str());
    this->timestampWriter.save(path.parent_path().string() + "/timestamps");
  }
}

boost::format
HardCamera::createFormat(const std::string& strFormat,
                         const unsigned hardCameraUid)
{
  try
  {
    return boost::format(strFormat);
  }
  catch (const boost::io::bad_format_string& e)
  {
    throw camera_parameter_error()
      << info_what("Syntax error on save image format string. Consult the "
                   "boost format library documentation.")
      << info_cameraUid(hardCameraUid);
  }
}

void
HardCamera::updateCurrentFrame(Frame& frame)
{
  static int unwarpedIdx = 0;

  cvRemap(frame.image, this->unwarpedFrameBuffer[unwarpedIdx].image,
          this->mapx, this->mapy,
          CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(0));
  this->unwarpedFrameBuffer[unwarpedIdx].timestamp = frame.timestamp;

  this->framesAccessMutex.lock();
  this->warpedFrame = &frame;
  this->unwarpedFrame = &this->unwarpedFrameBuffer[unwarpedIdx];
  this->framesAccessMutex.unlock();

  // Notify the classes observing this camera.
  notify();

  if (this->saveWarpedImages)
  {
    this->saveImageFormat % this->hardCameraUid % this->imagesCounter
                          % "w";
    cvSaveImage(this->saveImageFormat.str().c_str(),
                this->warpedFrame->image);
    if (this->saveTimestamp)
    {
      bfs::path path(this->saveImageFormat.str());
      this->timestampWriter.add(path.filename(),
                                this->warpedFrame->timestamp);
    }
  }

  if (this->saveUnwarpedImages)
  {
    this->saveImageFormat % this->hardCameraUid % this->imagesCounter
                          % "u";
    cvSaveImage(this->saveImageFormat.str().c_str(),
                this->unwarpedFrame->image);
    if (this->saveTimestamp)
    {
      bfs::path path(this->saveImageFormat.str());
      this->timestampWriter.add(path.filename(),
                                this->unwarpedFrame->timestamp);
    }
  }

  if (this->showImages)
  {
    cvShowImage(this->windowName.c_str(), this->unwarpedFrame->image);
    cvWaitKey(5);
  }

  unwarpedIdx = (unwarpedIdx+1) % 2;
  this->imagesCounter++;
}

void
HardCamera::getWarpedFrame(Frame& image)
{
  boost::shared_lock<boost::shared_mutex> lock(this->framesAccessMutex);
  image = *this->warpedFrame;
}

void
HardCamera::getUnwarpedFrame(Frame& image)
{
  boost::shared_lock<boost::shared_mutex> lock(this->framesAccessMutex);
  image = *this->unwarpedFrame;
}
