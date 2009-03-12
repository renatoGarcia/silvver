#ifndef HARD_CAMERA_FACTORY_HPP
#define HARD_CAMERA_FACTORY_HPP

#include <map>
#include <stdexcept>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include "hardCamera.hpp"
#include "scene.hpp"

class HardCameraFactory : public boost::noncopyable
{
public:

  static boost::shared_ptr<HardCamera>
  create(const scene::Camera& cameraConfig);

private:

  HardCameraFactory();

  /// The std::string key is the HardCamera unique identifier
  static std::map< std::string, boost::shared_ptr<HardCamera> >
  createdHardCameras;

  static boost::mutex mutexCameraCreate;
};

#endif /* HARD_CAMERA_FACTORY_HPP */
