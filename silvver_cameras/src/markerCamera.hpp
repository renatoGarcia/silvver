#ifndef MARKER_CAMERA_HPP
#define MARKER_CAMERA_HPP

#include <vector>
#include <string>
#include <boost/scoped_ptr.hpp>
#include "abstractCamera.hpp"
#include "extratorMarca.hpp"

/// Implementa uma câmera abstrata que filma marcos.
class MarkerCamera : public AbstractCamera
{
public:

  MarkerCamera(const std::vector<TargetConfig> &vecTargets,
               const CameraConfig& cameraConfig,
               const std::string& serverIP,
               unsigned connectionPort);

  ~MarkerCamera();

  virtual void operator()();

private:

  boost::scoped_ptr<MarkerExtractor> markerExtractor;
};

#endif
