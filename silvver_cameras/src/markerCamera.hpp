#ifndef MARKER_CAMERA_HPP
#define MARKER_CAMERA_HPP

#include <vector>
#include <string>
#include <boost/scoped_ptr.hpp>
#include "abstractCamera.hpp"
#include "markerExtractor.hpp"

/// Implementa uma câmera abstrata que filma marcos.
class MarkerCamera : public AbstractCamera
{
public:

  MarkerCamera(const std::vector<scene::Target> &vecTargets,
               const scene::Camera& cameraConfig,
               const std::string& serverIP,
               unsigned connectionPort);

  ~MarkerCamera();

  virtual void operator()();

private:

  boost::scoped_ptr<MarkerExtractor> markerExtractor;
};

#endif
