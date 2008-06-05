#ifndef HARDCAMERA_HPP
#define HARDCAMERA_HPP

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string>
#include <stdexcept>

class HardCamera
{
public:

  enum Resolution{
    RESOLUTION_640x480
  };

  enum FrameRate{
    FR_7_5,
    FR_15,
    FR_30
  };

  class OpenCameraFailed : public std::runtime_error
  {
  public:
    OpenCameraFailed(const std::string& whatArg)
      :runtime_error(whatArg){}
  };

  class CaptureImageError : public std::runtime_error
  {
  public:
    CaptureImageError(const std::string& whatArg)
      :runtime_error(whatArg){}
  };

  class OpenFileError : public std::runtime_error
  {
  public:
    OpenFileError(const std::string& whatArg)
      :runtime_error(whatArg){}
  };

  virtual ~HardCamera()=0;
  virtual void initialize() = 0;
  void createIplImage(IplImage* &iplImage);
  virtual void saveFrame() = 0;
  virtual void captureFrame(IplImage* &imgRetorno) = 0;

protected:

  /// Frame size in pixels
  unsigned frameSize;
  unsigned frameWidth, frameHeight;

  const uint64 UID;

  FrameRate frameRate;

  HardCamera(uint64 UID, Resolution resolution, FrameRate frameRate);

};

#endif
