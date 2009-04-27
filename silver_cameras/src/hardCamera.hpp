#ifndef HARD_CAMERA_HPP
#define HARD_CAMERA_HPP

#include <stdexcept>
#include <string>

#include <boost/array.hpp>
#include <boost/noncopyable.hpp>

#include <opencv/cv.h>
#include <opencv/highgui.h>

class HardCamera : public boost::noncopyable
{
public:

  class camera_parameter_error : public std::logic_error
  {
  public:
    camera_parameter_error(const std::string& whatArg)
      :logic_error(whatArg){}
  };

  class open_camera_error : public std::runtime_error
  {
  public:
    open_camera_error(const std::string& whatArg)
      :runtime_error(whatArg){}
  };

  class capture_image_error : public std::runtime_error
  {
  public:
    capture_image_error(const std::string& whatArg)
      :runtime_error(whatArg){}
  };

  virtual ~HardCamera();

  void createIplImage(IplImage* &iplImage) const;

  virtual void saveFrame() = 0;

  virtual void captureFrame(IplImage* &imgRetorno) = 0;

protected:

  HardCamera(const std::string& uid,
             const boost::array<unsigned, 2>& resolution,
             float frameRate);

  // A string representing the uid of the hardware camera.
  // It must be numbers in decimal base.
  const std::string uid;

  // Frame size measures in pixels.
  const unsigned frameSize;
  const unsigned frameWidth, frameHeight;

  const float frameRate;

private:

  friend class HardCameraFactory;

  virtual void initialize() = 0;
};

#endif
