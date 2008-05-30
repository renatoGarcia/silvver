#include "markerCamera.hpp"

MarkerCamera::MarkerCamera(CameraConfig cameraConfig, std::string serverIP,
                           unsigned connectionPort)
  :AbstractCamera(cameraConfig, serverIP, connectionPort)
{
  this->extratorMarca.reset(new ExtratorMarca(cameraConfig.resolucao.at(0),
                                              cameraConfig.resolucao.at(1)));
}

MarkerCamera::~MarkerCamera()
{}

void
MarkerCamera::operator()()
{
  this->connect();

  this->startHardCamera();

  std::vector<Ente> vecEnte;
  Pacote<Ente> pacote(UID);
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

      iteMarkerPontos->centro.setTempo(0);
      teta = iteMarkerPontos->verticeRef.CalcularAngulo(iteMarkerPontos->verticeSec);

      vecEnte.push_back( Ente(iteMarkerPontos->centro,teta) );
    }

    pacote.Empacotar(vecEnte);
    this->connection->send(&pacote,sizeof(pacote));
  }
}
