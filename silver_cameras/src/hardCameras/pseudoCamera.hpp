#ifndef PSEUDO_CAMERA_HPP
#define PSEUDO_CAMERA_HPP

#include "../hardCamera.hpp"

#include <string>

#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>

class PseudoCamera : public HardCamera
{
public:

  PseudoCamera(const scene::PseudoCamera& config);

  ~PseudoCamera();

  void initialize();

  void saveFrame();

  // Throw capture_image_error.
  void captureFrame(IplImage** iplImage, unsigned clientUid);

private:

  const boost::filesystem::path path;

  const float frameRate;

  boost::filesystem::directory_iterator dirIterator, endIterator;

  // Tempo que pseudoCamera ficará ociosa para simular uma dada frequência
  const boost::posix_time::millisec delay;

  boost::mutex mutexCaptureFrame;
};

#endif
