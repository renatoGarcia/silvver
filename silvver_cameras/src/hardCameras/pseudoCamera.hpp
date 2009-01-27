#ifndef PSEUDOCAMERA_HPP
#define PSEUDOCAMERA_HPP

#include "../hardCamera.hpp"

#include <string>

#include <boost/array.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>

namespace bfs = boost::filesystem;

class PseudoCamera : public HardCamera
{
public:

  PseudoCamera(const std::string& uid,
               const boost::array<unsigned, 2>& resolution,
               float frameRate,
               const std::string& imagesPath);

  ~PseudoCamera();

  virtual void initialize();

  virtual void saveFrame();

  virtual void captureFrame(IplImage* &iplImage);

private:

  const bfs::path PATH;

  bfs::directory_iterator dirIterator, endIterator;

  // Tempo que pseudoCamera ficará ociosa para simular uma dada frequência
  unsigned delay;

  boost::mutex mutexCaptureFrame;
};

#endif
