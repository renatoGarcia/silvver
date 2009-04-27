#ifndef MARKER_CAMERA_HPP
#define MARKER_CAMERA_HPP

#include <vector>
#include <string>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "abstractCamera.hpp"
#include "connection.hpp"
#include "markerExtractor.hpp"

/// Implements an abstract camerata which look for artp markers
class MarkerCamera : public AbstractCamera
{
public:

  MarkerCamera(const std::vector<scene::Target>& vecTargets,
               const scene::Camera& cameraConfig,
               boost::shared_ptr<Connection> connection);

  ~MarkerCamera();

  virtual void operator()();

private:

  const boost::scoped_ptr<MarkerExtractor> markerExtractor;
};

#endif
