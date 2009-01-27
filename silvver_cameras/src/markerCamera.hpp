#ifndef MARKER_CAMERA_HPP
#define MARKER_CAMERA_HPP

#include <vector>
#include <string>

#include <boost/scoped_ptr.hpp>

#include "abstractCamera.hpp"
#include "markerExtractor.hpp"

/// Implements an abstract camerata which look for artp markers
class MarkerCamera : public AbstractCamera
{
public:

  MarkerCamera(const std::vector<scene::Target>& vecTargets,
               const scene::Camera& cameraConfig,
               const std::string& serverIP,
               unsigned connectionPort);

  ~MarkerCamera();

private:

  virtual void operator()();

  const boost::scoped_ptr<MarkerExtractor> markerExtractor;
};

#endif
