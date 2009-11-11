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

#include "artkpCamera.hpp"

#include <boost/foreach.hpp>
#include <fstream>

#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>

#include "../connection.ipp"
#include <silvverTypes.hpp>

ArtkpCamera::ArtkpCamera(const scene::Camera& cameraConfig,
                         const scene::ArtkpTargets& targets,
                         boost::shared_ptr<Connection> connection,
                         bool showImages)
  :AbstractCamera(cameraConfig, connection, showImages)
  ,camConfigFileName("/tmp/artkpCamera" +
                     boost::apply_visitor(scene::GetHardware(),
                                          cameraConfig.hardware).uid)
  ,patternWidth(targets.patternWidth)
  ,threshold(targets.threshold)
  ,logger()
  ,tracker(new ARToolKitPlus::TrackerSingleMarkerImpl<16,16,64,50,50>
           (this->currentFrame->width, this->currentFrame->height))
  ,runThread()
{
  int targetNum = 0;
  boost::tuple<unsigned, std::string> pattern;
  BOOST_FOREACH(pattern, targets.patterns)
  {
    this->vecDescriptionFilePath.push_back(boost::get<1>(pattern));
    this->idMap.at(targetNum) = boost::get<0>(pattern); // Get silvver uid
    targetNum++;
  }

  const scene::Hardware hardwareConfig =
    boost::apply_visitor(scene::GetHardware(), cameraConfig.hardware);

  std::ofstream tmpConfig(this->camConfigFileName.c_str());
  tmpConfig.precision(10);
  tmpConfig << "ARToolKitPlus_CamCal_Rev02" << "\n"
            << this->currentFrame->width << " "
            << this->currentFrame->height << " "
            << hardwareConfig.principalPoint.at(0) << " "
            << hardwareConfig.principalPoint.at(1) << " "
            << hardwareConfig.focalLength.at(0) << " "
            << hardwareConfig.focalLength.at(1) << " "
            << 0.0 << " " << 0.0 << " " << 0.0 << " "
            << 0.0 << " " << 0.0 << " " << 0.0 << " "
            << 0;
  tmpConfig.close();

  if(!this->tracker->init(this->camConfigFileName.c_str(),
                          1000.0f,
                          7000.0f,
                          &this->logger))
  {
    throw initialize_error(this->logger.message);
  }

  this->tracker->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_BGR);

  this->tracker->setPatternWidth(this->patternWidth);

  // Define the proportion of bord in image size.
  this->tracker->setBorderWidth(1.0/6.0);

  this->tracker->setThreshold(this->threshold);

  this->tracker->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);

  this->tracker->setUndistortionMode(ARToolKitPlus::UNDIST_NONE);

  this->tracker->setMarkerMode(ARToolKitPlus::MARKER_TEMPLATE);

  this->tracker->setImageProcessingMode(ARToolKitPlus::IMAGE_FULL_RES);

  BOOST_FOREACH(std::string filePath, this->vecDescriptionFilePath)
  {
    this->tracker->addPattern(filePath.c_str());
  }
}

ArtkpCamera::~ArtkpCamera()
{
  if (this->runThread)
  {
    this->runThread->interrupt();
    this->runThread->join();
  }
}

void
ArtkpCamera::run()
{
  this->runThread.reset(new boost::thread(boost::bind<void>
                                          (&ArtkpCamera::doWork,
                                           this)));
}

void
ArtkpCamera::stop()
{
  if (this->runThread)
  {
    this->runThread->interrupt();
    this->runThread->join();
  }
}

void
ArtkpCamera::doWork()
{
  std::vector<silvver::Identity<silvver::Pose> > poses;

  int nMarkers = 0;
  float error = 0;
  ARToolKitPlus::ARMarkerInfo* markerInfo;
  ARFloat pattCenter[2] = {0.0, 0.0};
  ARFloat transMatrix[3][4];
  silvver::Identity<silvver::Pose> pose;

  while(true)
  {
    boost::this_thread::interruption_point();

    poses.clear();
    try
    {
      this->updateFrame();
    }
    catch (const HardCamera::capture_image_error& exception)
    {
      std::cerr << exception.what() << std::endl;
      return;
    }

    nMarkers = 0;

    if (this->tracker->arDetectMarker((ARToolKitPlus::ARUint8*)this->currentFrame->imageData,
                                      this->threshold,
                                      &markerInfo,
                                      &nMarkers)
        < 0 )
    {
      throw detect_marker_error("Error in arDetectMarker method.");
    }
    for (int marker = 0; marker < nMarkers; marker++)
    {
      // If the marker was not identified or its confidence is lower than 50%
      if ((markerInfo[marker].id < 0) || (markerInfo[marker].cf<0.5))
      {
        continue;
      }

      // The transMatrix will be the marker pose in relation to camera.
      error = this->tracker->rppGetTransMat(&markerInfo[marker],
                                    pattCenter,
                                    (ARFloat)this->patternWidth,
                                    transMatrix);

      // If the error in the pose estimated is 1e10, the rppGetTransMat has
      // failed.
      if (error == 1e10)
      {
        error = this->tracker->arGetTransMat(&markerInfo[marker],
                                             pattCenter,
                                             (ARFloat)this->patternWidth,
                                             transMatrix);
      }

      pose.uid = this->idMap.at(markerInfo[marker].id);
      pose.x = transMatrix[0][3];
      pose.y = transMatrix[1][3];
      pose.z = transMatrix[2][3];
      for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
          pose.rotationMatrix[(3*i)+j] = transMatrix[i][j];

      this->toWorld(pose);

      poses.push_back(pose);
    }

    this->serverConnection->send(poses);
  }
}
