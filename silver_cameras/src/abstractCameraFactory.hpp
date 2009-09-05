#ifndef ABSTRACT_CAMERA_FACTORY_HPP
#define ABSTRACT_CAMERA_FACTORY_HPP

#include <vector>
#include <string>

#include "abstractCamera.hpp"
#include "connection.hpp"
#include "scene.hpp"

class AbstractCameraFactory
{
public:

  static AbstractCamera*
  create(const scene::Camera& cameraConfig,
         const scene::VariantAnyTarget& targets,
         boost::shared_ptr<Connection> connection);

private:

  AbstractCameraFactory();
};


#endif /* ABSTRACT_CAMERA_FACTORY_HPP */
