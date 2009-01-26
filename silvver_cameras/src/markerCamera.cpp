#include "markerCamera.hpp"

MarkerCamera::MarkerCamera(const std::vector<scene::Target> &vecTargets,
                           const scene::Camera& cameraConfig,
                           const std::string& serverIP,
                           unsigned connectionPort)
  :AbstractCamera(cameraConfig, serverIP, connectionPort)
{
  this->targetType = AbstractCamera::ARTP_MARK;

  this->markerExtractor.reset(new MarkerExtractor(cameraConfig.resolution.at(0),
                                                  cameraConfig.resolution.at(1),
                                                  vecTargets));
}

MarkerCamera::~MarkerCamera()
{}

void
MarkerCamera::operator()()
{
  this->connect();

  this->markerExtractor->init();

  std::vector<silver::Ente> vecEnte;
  silver::Package<silver::Ente> package;
  std::vector<MarkerPoints> vecMarkerPoints;
  std::vector<MarkerPoints>::iterator iteMarkerPoints;
  double theta;
  while(!stopping)
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
    this->connection->send(&package,sizeof(package));
  }
}
