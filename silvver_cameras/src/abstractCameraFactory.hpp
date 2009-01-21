#ifndef ABSTRACT_CAMERA_FACTORY_HPP
#define ABSTRACT_CAMERA_FACTORY_HPP

#include "abstractCamera.hpp"
#include "scene.hpp"
#include <boost/noncopyable.hpp>
#include <vector>
#include <string>

class AbstractCameraFactory : public boost::noncopyable
{
public:

  static AbstractCamera*
  create(const std::string& targetType,
         const std::vector<TargetConfig> &vecTargets,
         const CameraConfig& cameraConfig,
         const std::string& serverIP,
         unsigned connectionPort);

private:

  AbstractCameraFactory();

};


#endif /* ABSTRACT_CAMERA_FACTORY_HPP */
