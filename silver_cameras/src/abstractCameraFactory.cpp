#include "abstractCameraFactory.hpp"

// #include "blobCamera.hpp"
#include "markerCamera.hpp"

AbstractCamera*
AbstractCameraFactory::create(const std::string& targetType,
                              const std::vector<scene::Target> &vecTargets,
                              const scene::Camera& cameraConfig,
                              boost::shared_ptr<Connection> connection)
{
  AbstractCamera* abstractCamera;

  if(targetType == "artp_mark")
  {
    abstractCamera = new MarkerCamera(vecTargets,
                                      cameraConfig,
                                      connection);
  }
//   else if(targetType == "color_blob")
//   {
//     abstractCamera = new BlobCamera(vecTargets,
//                                     cameraConfig,
//                                     connection);
//   }
  else
  {
    throw std::invalid_argument("Unknown target type: " + targetType);
  }

  return abstractCamera;
}
