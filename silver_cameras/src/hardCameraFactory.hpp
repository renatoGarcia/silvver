#ifndef HARD_CAMERA_FACTORY_HPP
#define HARD_CAMERA_FACTORY_HPP

#include <map>
#include <stdexcept>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/tuple/tuple.hpp>

#include "hardCamera.hpp"
#include "scene.hpp"

class HardCameraFactory : public boost::noncopyable
{
public:

  /** Return a shared_ptr to the hardCamera described in cameraConfig and uid.
   * Return the shared_ptr and the client uid related to that hardware camera.
   * @param cameraConfig The struct describing the hardware camera.
   * @return A tuple composed by hardCamera and clientUid.  HardCamera is a
   *         shared_ptr which will point to the required hardware camera, and
   *         the clientUid is an unsigned with the uid of calling object.
   */
   static boost::tuple<boost::shared_ptr<HardCamera>, unsigned>
   create(const scene::Camera& cameraConfig);

private:

  HardCameraFactory();

  /// The std::string key is the HardCamera unique identifier
  static std::map< std::string, boost::shared_ptr<HardCamera> >
  createdHardCameras;

  static boost::mutex mutexCameraCreate;
};

#endif /* HARD_CAMERA_FACTORY_HPP */
