#ifndef ABSTRACT_CAMERA_FACTORY_HPP
#define ABSTRACT_CAMERA_FACTORY_HPP

#include "abstractCamera.hpp"

#include <vector>
#include <string>

#include "scene.hpp"

class AbstractCameraFactory
{
public:

  static AbstractCamera*
  create(const std::string& targetType,
         const std::vector<scene::Target> &vecTargets,
         const scene::Camera& cameraConfig,
         const std::string& serverIP,
         unsigned connectionPort);

private:

  AbstractCameraFactory();
};


#endif /* ABSTRACT_CAMERA_FACTORY_HPP */
