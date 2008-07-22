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

std::map < uint64,/*boost::shared_ptr<HardCamera>*/HardCamera* >
HardCameraFactory::createdHardCameras;

boost::mutex
HardCameraFactory::mutexCameraCreate;

HardCameraFactory::HardCameraFactory()
{}

HardCamera*
HardCameraFactory::createHardCamera(CameraConfig cameraConfig)
{
  boost::mutex::scoped_lock lock(HardCameraFactory::mutexCameraCreate);

//   boost::shared_ptr<HardCamera> hardCameraPtr =
//     HardCameraFactory::createdHardCameras[cameraConfig.uid];

//   // Se já criado
//   if(hardCameraPtr)
//   {
//     return hardCameraPtr.get();
//   }

  std::map < uint64,HardCamera* >::iterator iteHardCamera =
    HardCameraFactory::createdHardCameras.find(cameraConfig.uid);

  // Se já criado
  if(iteHardCamera != HardCameraFactory::createdHardCameras.end())
  {
    return HardCameraFactory::createdHardCameras[cameraConfig.uid];
  }

  HardCamera* hardCameraPtr;


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

  if(cameraConfig.hardware == "pseudocamera")
  {
//     hardCameraPtr.reset(new PseudoCamera(cameraConfig.uid, frameRate,
//                                          resolution, cameraConfig.imagesPath))

    hardCameraPtr = new PseudoCamera(cameraConfig.uid, frameRate,
                                     resolution, cameraConfig.imagesPath);
  }
#ifdef HAVE_LIBDC1394_DC1394_CONTROL_H
  else if(cameraConfig.hardware == "dc1394")
  {
//     hardCameraPtr.reset(new DC1394(0, cameraConfig.uid,
//                                    frameRate, resolution));

    hardCameraPtr = new DC1394(0, cameraConfig.uid,
                               frameRate, resolution);
  }
#endif
  else
  {
    throw CameraParameterError("Unknown hardware camera model: " +
                               cameraConfig.hardware);
  }

  hardCameraPtr->initialize();

  return hardCameraPtr;
}
