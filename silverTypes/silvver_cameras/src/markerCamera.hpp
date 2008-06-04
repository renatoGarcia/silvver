#ifndef MARKER_CAMERA_HPP
#define MARKER_CAMERA_HPP

#include <vector>
#include <string>
#include <boost/scoped_ptr.hpp>
#include "abstractCamera.hpp"
#include "extratorMarca.hpp"

using namespace silver;

/// Implementa uma câmera abstrata que filma marcos.
class MarkerCamera : public AbstractCamera
{
public:

  MarkerCamera(CameraConfig cameraConfig, std::string serverIP,
               unsigned connectionPort);

  virtual ~MarkerCamera();

  virtual void operator()();

private:

  boost::scoped_ptr<ExtratorMarca> extratorMarca;
};

#endif
