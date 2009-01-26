#include "hardCameraFactory.hpp"
#include <boost/lexical_cast.hpp>

#include "hardCameras/pseudoCamera.hpp"

#ifdef HAVE_CONFIG_H
 #include "../config.h"
#endif
#ifdef HAVE_PGRFLYCAPTURE_HEADERS
 #include "hardCameras/pgr.hpp"
#endif
#ifdef HAVE_LIBDC1394_DC1394_CONTROL_H
 #include "hardCameras/dc1394.hpp"
#endif

std::map < std::string, boost::shared_ptr<HardCamera> >
HardCameraFactory::createdHardCameras;

boost::mutex
HardCameraFactory::mutexCameraCreate;

HardCameraFactory::HardCameraFactory()
{}

boost::shared_ptr<HardCamera>
HardCameraFactory::createHardCamera(scene::Camera cameraConfig)
{
  boost::mutex::scoped_lock lock(HardCameraFactory::mutexCameraCreate);

  std::map< std::string, boost::shared_ptr<HardCamera> >::iterator
    iteHardCamera = HardCameraFactory::createdHardCameras.find(cameraConfig.uid);

  // if hardCamera already created
  if(iteHardCamera != HardCameraFactory::createdHardCameras.end())
  {
    return iteHardCamera->second;
  }

  HardCamera::FrameRate frameRate;
  if(cameraConfig.frameRate == 7.5)
  {
    frameRate = HardCamera::FR_7_5;
  }
  else if(cameraConfig.frameRate == 15.0)
  {
    frameRate = HardCamera::FR_15;
  }
  else if(cameraConfig.frameRate == 30.0)
  {
    frameRate = HardCamera::FR_30;
  }
  else
  {
    throw CameraParameterError("Invalid frame rate: " +
                               boost::lexical_cast<std::string>
                               (cameraConfig.frameRate) );
  }

  HardCamera::Resolution resolution;
  if(cameraConfig.resolution.at(0) == 640 &&
     cameraConfig.resolution.at(1) == 480)
  {
    resolution = HardCamera::RESOLUTION_640x480;
  }
  else
  {
    throw CameraParameterError("Invalid resolution: "
                               +
                               boost::lexical_cast<std::string>
                               (cameraConfig.resolution.at(0))
                               + "x" +
                               boost::lexical_cast<std::string>
                               (cameraConfig.resolution.at(1))
                               );
  }

  boost::shared_ptr<HardCamera> hardCameraPtr;

  if(cameraConfig.hardware == "pseudocamera")
  {
    hardCameraPtr.reset(new PseudoCamera(cameraConfig.uid, frameRate,
                                         resolution, cameraConfig.imagesPath));
  }
#ifdef HAVE_LIBDC1394_DC1394_CONTROL_H
  else if(cameraConfig.hardware == "dc1394")
  {
    hardCameraPtr.reset(new DC1394(0, cameraConfig.uid,
                                   frameRate, resolution));
  }
#endif
  else
  {
    throw CameraParameterError("Unknown hardware camera model: " +
                               cameraConfig.hardware);
  }

  hardCameraPtr->initialize();

  HardCameraFactory::createdHardCameras.
    insert(std::pair< std::string, boost::shared_ptr<HardCamera> >
           (cameraConfig.uid, hardCameraPtr));

  return hardCameraPtr;
}
