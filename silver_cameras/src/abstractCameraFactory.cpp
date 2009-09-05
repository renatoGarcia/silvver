#include "abstractCameraFactory.hpp"

// #include "blobCamera.hpp"
// #include "markerCamera.hpp"
#include "artkpCamera.hpp"

AbstractCamera*
AbstractCameraFactory::create(const scene::Camera& cameraConfig,
                              const scene::VariantAnyTarget& targets,
                              boost::shared_ptr<Connection> connection)
{
  AbstractCamera* abstractCamera;

  if (const scene::ArtkpTargets* artkpTarget =
      boost::get<scene::ArtkpTargets>(&targets))
  {
    abstractCamera = new ArtkpCamera(cameraConfig,
                                     *artkpTarget,
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
    throw std::invalid_argument("Unknown target type");
  }

  return abstractCamera;
}
