#ifndef DC1394_HPP
#define DC1394_HPP

#include "../hardCamera.hpp"
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>
#include <sys/time.h>
#include <string>
#include "conversions.h"

#define NUMERO_BUFFERS 4

class DC1394: public HardCamera
{
public:


  enum Mode{
    _640x480_MONO = MODE_640x480_MONO,
  };

  enum Format{
    F_VGA_NONCOMPRESSED = FORMAT_VGA_NONCOMPRESSED,
  };

  DC1394(int nCard, unsigned identifier, FrameRate frameRate,
         Mode mode = _640x480_MONO,
         Format format = F_VGA_NONCOMPRESSED);

  ~DC1394();

  virtual void initialize();

  virtual void captureFrame(IplImage* iplImage);

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
  Mode mode;
  int bytesPerPixel;

  bayer_pattern_t pattern;
};

#endif
