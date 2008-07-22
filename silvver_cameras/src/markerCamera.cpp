#include "markerCamera.hpp"

MarkerCamera::MarkerCamera(CameraConfig cameraConfig, std::string serverIP,
                           unsigned connectionPort)
  :AbstractCamera(cameraConfig, serverIP, connectionPort)
{
  this->targetType = AbstractCamera::ARTP_MARK;

  this->extratorMarca.reset(new ExtratorMarca(cameraConfig.resolution.at(0),
                                              cameraConfig.resolution.at(1)));
}

MarkerCamera::~MarkerCamera()
{}

void
MarkerCamera::operator()()
{
  this->connect();

  this->extratorMarca->Iniciar();

  std::vector<Ente> vecEnte;
  Package<Ente> pacote;
  std::vector<MarkerPontos> vecMarkerPontos;
  std::vector<MarkerPontos>::iterator iteMarkerPontos;
  double teta;
  while(!stopping)
  {
    vecEnte.clear();
    this->updateFrame();

    this->extratorMarca->ExtrairMarcas(this->actualFrame, vecMarkerPontos);

    iteMarkerPontos = vecMarkerPontos.begin();
    for(;iteMarkerPontos<vecMarkerPontos.end();iteMarkerPontos++)
    {
      this->localize(iteMarkerPontos->centro);
      this->localize(iteMarkerPontos->verticeRef);
      this->localize(iteMarkerPontos->verticeSec);

      teta = iteMarkerPontos->verticeRef.findAngle(iteMarkerPontos->verticeSec);

      vecEnte.push_back( Ente(iteMarkerPontos->centro,teta) );
    }

    pacote.pack(vecEnte);
    this->connection->send(&pacote,sizeof(pacote));
  }
}
