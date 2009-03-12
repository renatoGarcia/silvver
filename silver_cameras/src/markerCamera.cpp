#include "markerCamera.hpp"

#include "connection.ipp"
#include "silverTypes.hpp"

MarkerCamera::MarkerCamera(const std::vector<scene::Target> &vecTargets,
                           const scene::Camera& cameraConfig,
                           boost::shared_ptr<Connection> connection)
  :AbstractCamera(cameraConfig, connection)
  ,markerExtractor(new MarkerExtractor(cameraConfig.resolution.at(0),
                                       cameraConfig.resolution.at(1),
                                       vecTargets))
{}

MarkerCamera::~MarkerCamera()
{}

void
MarkerCamera::operator()()
{
  this->markerExtractor->initialize();

  std::vector<silver::Ente> vecEnte;
  silver::Package<silver::Ente> package;
  std::vector<MarkerExtractor::MarkerPoints> vecMarkerPoints;
  std::vector<MarkerExtractor::MarkerPoints>::iterator iteMarkerPoints;
  double theta;
  while(!this->stopping)
  {
    vecEnte.clear();
    this->updateFrame();

    this->markerExtractor->extract(this->currentFrame, vecMarkerPoints);

    iteMarkerPoints = vecMarkerPoints.begin();
    for (; iteMarkerPoints<vecMarkerPoints.end(); iteMarkerPoints++)
    {
      this->localize(iteMarkerPoints->center);
      this->localize(iteMarkerPoints->primaryVertex);
      this->localize(iteMarkerPoints->secondaryVertex);

      theta =
        iteMarkerPoints->primaryVertex.findAngle(iteMarkerPoints->
                                                 secondaryVertex);

      iteMarkerPoints->center.theta = theta;
      vecEnte.push_back(iteMarkerPoints->center);
    }

    this->serverConnection->send(vecEnte);
  }
}
