#ifndef DC1394_HPP
#define DC1394_HPP

#include "../hardCamera.hpp"
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>
#include <sys/time.h>
#include <string>
#include <boost/thread/mutex.hpp>
#include "conversions.h"

#define NUMERO_BUFFERS 4

class DC1394: public HardCamera
{
public:

  enum Format{
    F_VGA_NONCOMPRESSED = FORMAT_VGA_NONCOMPRESSED,
  };

  DC1394(int nCard, uint64 identifier,
         HardCamera::FrameRate frameRate,
         HardCamera::Resolution resolution,
         Format format = F_VGA_NONCOMPRESSED);

  virtual ~DC1394();

  virtual void initialize();

  virtual void captureFrame(IplImage* &iplImage);

  // Grava a última imagem da câmera no disco
  virtual void saveFrame();

  void createIplImage(IplImage* &iplImage);

private:

  int nCard;
  std::string device;

  raw1394handle_t raw1394Handle;
  bool            bRaw1394HandleCreated;

  dc1394_cameracapture dc1394Camera;
  bool                 bDc1394CameraCreated;

  Format format;
  int mode;
  int bytesPerPixel;

  bayer_pattern_t pattern;

  boost::mutex mutexCaptureFrame;
};

#endif