#include "markerCamera.hpp"

#include "silverTypes.hpp"

MarkerCamera::MarkerCamera(const std::vector<scene::Target> &vecTargets,
                           const scene::Camera& cameraConfig,
                           const std::string& serverIP,
                           unsigned connectionPort)
  :AbstractCamera(cameraConfig, serverIP, connectionPort, silver::ARTP_MARK)
  ,markerExtractor(new MarkerExtractor(cameraConfig.resolution.at(0),
                                       cameraConfig.resolution.at(1),
                                       vecTargets))
{}

MarkerCamera::~MarkerCamera()
{}

void
MarkerCamera::operator()()
{
  this->connect();

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
    for(;iteMarkerPoints<vecMarkerPoints.end();iteMarkerPoints++)
    {
      this->localize(iteMarkerPoints->centro);
      this->localize(iteMarkerPoints->verticeRef);
      this->localize(iteMarkerPoints->verticeSec);

      theta = iteMarkerPoints->verticeRef.findAngle(iteMarkerPoints->verticeSec);

      iteMarkerPoints->centro.theta = theta;
      vecEnte.push_back(iteMarkerPoints->centro);
    }

    package.pack(vecEnte);
    this->serverConnection->send(&package,sizeof(package));
  }
}
