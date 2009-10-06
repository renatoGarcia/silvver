#include "hardCameraFactory.hpp"

#include "hardCameras/pseudoCamera.hpp"

#ifdef HAVE_CONFIG_H
 #include "../config.h"
#endif
#ifdef HAVE_PGRFLYCAPTURE_HEADERS
 #include "hardCameras/pgr.hpp"
#endif
#ifdef HAVE_LIBDC1394_DC1394_CONTROL_H
 #include "hardCameras/dc1394_1x.hpp"
 #define HAVE_DC1394
#endif
#ifdef HAVE_DC1394_DC1394_H
 #include "hardCameras/dc1394_2x.hpp"
 #define HAVE_DC1394
#endif

std::map < std::string, boost::shared_ptr<HardCamera> >
HardCameraFactory::createdHardCameras;

boost::mutex
HardCameraFactory::mutexCameraCreate;

boost::tuple<boost::shared_ptr<HardCamera>, unsigned>
HardCameraFactory::create(const scene::Camera& cameraConfig)
{
  // HardCameras can't be simultaneously initialized
  boost::mutex::scoped_lock lock(HardCameraFactory::mutexCameraCreate);

  boost::tuple<boost::shared_ptr<HardCamera>, unsigned> hardCamera;

  std::map< std::string, boost::shared_ptr<HardCamera> >::iterator
    iteHardCamera = HardCameraFactory::createdHardCameras.find(cameraConfig.uid);

  // If hardCamera is already created
  if(iteHardCamera != HardCameraFactory::createdHardCameras.end())
  {
    hardCamera.get<0>() = iteHardCamera->second;
  }
  else
  {
    if (cameraConfig.hardware == "pseudocamera")
    {
      hardCamera.get<0>().reset(new PseudoCamera(cameraConfig));
    }
#ifdef HAVE_DC1394
    else if (cameraConfig.hardware == "dc1394")
    {
      hardCamera.get<0>().reset(new DC1394(cameraConfig));
    }
#endif
    else
    {
      throw HardCamera::camera_parameter_error("Unknown hardware camera model: " +
                                               cameraConfig.hardware);
    }

    hardCamera.get<0>()->initialize();

    HardCameraFactory::createdHardCameras.
      insert(std::pair< std::string, boost::shared_ptr<HardCamera> >
             (cameraConfig.uid, hardCamera.get<0>()));
  }

  hardCamera.get<1>() = hardCamera.get<0>()->addClient();

  return hardCamera;
}
