#include "abstractCameraFactory.hpp"

#include "blobCamera.hpp"
#include "markerCamera.hpp"

AbstractCamera*
AbstractCameraFactory::create(const std::string& targetType,
                              const std::vector<scene::Target> &vecTargets,
                              const scene::Camera& cameraConfig,
                              const std::string& serverIP,
                              unsigned connectionPort)
{
  AbstractCamera* abstractCamera;

  if(targetType == "artp_mark")
  {
    abstractCamera = new MarkerCamera(vecTargets,
                                      cameraConfig,
                                      serverIP,
                                      connectionPort);
  }
  else if(targetType == "color_blob")
  {
    abstractCamera = new BlobCamera(vecTargets,
                                    cameraConfig,
                                    serverIP,
                                    connectionPort);
  }
  else
  {
    throw std::invalid_argument("Unknown target type: " + targetType);
  }

  return abstractCamera;
}
