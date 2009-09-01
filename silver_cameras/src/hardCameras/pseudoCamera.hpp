#ifndef PSEUDO_CAMERA_HPP
#define PSEUDO_CAMERA_HPP

#include "../hardCamera.hpp"

#include <string>

#include <boost/array.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>

class PseudoCamera : public HardCamera
{
public:

  PseudoCamera(const std::string& uid,
               const boost::array<unsigned, 2>& resolution,
               float frameRate,
               const std::string& imagesPath);

  ~PseudoCamera();

  void initialize();

  void saveFrame();

  // Throw capture_image_error.
  void captureFrame(IplImage** iplImage);

private:

  const boost::filesystem::path path;

  boost::filesystem::directory_iterator dirIterator, endIterator;

  // Tempo que pseudoCamera ficará ociosa para simular uma dada frequência
  const boost::posix_time::millisec delay;

  boost::mutex mutexCaptureFrame;
};

#endif
