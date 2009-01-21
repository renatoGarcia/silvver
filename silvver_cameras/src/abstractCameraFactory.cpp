#include "abstractCameraFactory.hpp"
#include "markerCamera.hpp"
#include "blobCamera.hpp"

AbstractCamera*
AbstractCameraFactory::create(const std::string& targetType,
                              const std::vector<TargetConfig> &vecTargets,
                              const CameraConfig& cameraConfig,
                              const std::string& serverIP,
                              unsigned connectionPort)
{
  AbstractCamera* ret;

  if(targetType == "artp_mark")
  {
    ret = new MarkerCamera(vecTargets,
                           cameraConfig,
                           serverIP,
                           connectionPort);
  }
  else if(targetType == "color_blob")
  {
    ret = new BlobCamera(vecTargets,
                         cameraConfig,
                         serverIP,
                         connectionPort);

  }
  else
  {
    throw std::invalid_argument("Unknown target type: " + targetType);
  }

  return ret;
}
