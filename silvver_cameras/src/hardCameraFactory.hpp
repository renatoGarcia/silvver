#ifndef HARDCAMERAFACTORY_HPP
#define HARDCAMERAFACTORY_HPP

#include "hardCamera.hpp"
#include "xmlParser.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <map>
#include <string>
#include <stdexcept>

class HardCameraFactory
{
public:

  class CameraParameterError : public std::logic_error
  {
  public:
    CameraParameterError(const std::string& whatArg)
      :logic_error(whatArg){}
  };

  static boost::shared_ptr<HardCamera>
  createHardCamera(CameraConfig cameraConfig);

private:

  HardCameraFactory();

  static std::map < uint64,boost::shared_ptr<HardCamera> > createdHardCameras;

  static boost::mutex mutexCameraCreate;


};

#endif