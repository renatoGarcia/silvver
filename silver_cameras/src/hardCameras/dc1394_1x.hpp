#ifndef DC1394_HPP
#define DC1394_HPP

#include "../hardCamera.hpp"

#include <string>

#include <boost/thread/mutex.hpp>

#include <libdc1394/dc1394_control.h>
#include <libraw1394/raw1394.h>
#include <sys/time.h>

#include "conversions.h"

class DC1394: public HardCamera
{
public:

  enum Format
  {
    F_VGA_NONCOMPRESSED = FORMAT_VGA_NONCOMPRESSED
  };

  DC1394(const scene::Camera& config,
         Format format = F_VGA_NONCOMPRESSED);

  ~DC1394();

  void initialize();

  void captureFrame(IplImage** iplImage);

  // Grava a última imagem da câmera no disco
  void saveFrame();

private:

  static const int NUMERO_BUFFERS = 4;

  const int nCard;
  const std::string device;

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
