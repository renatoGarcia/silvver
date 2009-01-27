#include "hardCameraFactory.hpp"

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

boost::shared_ptr<HardCamera>
HardCameraFactory::create(const scene::Camera& cameraConfig)
{
  // HardCameras can't be simultaneously initialized
  boost::mutex::scoped_lock lock(HardCameraFactory::mutexCameraCreate);

  std::map< std::string, boost::shared_ptr<HardCamera> >::iterator
    iteHardCamera = HardCameraFactory::createdHardCameras.find(cameraConfig.uid);

  boost::shared_ptr<HardCamera> hardCameraPtr;

  // If hardCamera is already created
  if(iteHardCamera != HardCameraFactory::createdHardCameras.end())
  {
    hardCameraPtr = iteHardCamera->second;
  }
  else
  {
    if (cameraConfig.hardware == "pseudocamera")
    {
      hardCameraPtr.reset(new PseudoCamera(cameraConfig.uid,
                                           cameraConfig.resolution,
                                           cameraConfig.frameRate,
                                           cameraConfig.imagesPath));
    }
#ifdef HAVE_LIBDC1394_DC1394_CONTROL_H
    else if (cameraConfig.hardware == "dc1394")
    {
      hardCameraPtr.reset(new DC1394(0, cameraConfig.uid,
                                     cameraConfig.frameRate,
                                     cameraConfig.resolution));
    }
#endif
    else
    {
      throw HardCamera::camera_parameter_error("Unknown hardware camera model: " +
                                               cameraConfig.hardware);
    }

    hardCameraPtr->initialize();

    HardCameraFactory::createdHardCameras.
      insert(std::pair< std::string, boost::shared_ptr<HardCamera> >
             (cameraConfig.uid, hardCameraPtr));
  }

  return hardCameraPtr;
}
