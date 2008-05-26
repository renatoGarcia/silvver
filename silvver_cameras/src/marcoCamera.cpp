#include "marcoCamera.hpp"

MarcoCamera::MarcoCamera(const CameraConfig& camConfig,double tempoInicial)
 :Camera(camConfig,tempoInicial)
{
  extratorMarca.reset(new ExtratorMarca(camConfig.resolucao[0],camConfig.resolucao[1]));
}

int MarcoCamera::Iniciar()
{
  try
  {
    Camera::Iniciar();
    this->numLogger = extratorMarca->Iniciar();
  }
  catch(std::string erro)
  {
    std::cout << "Erro: " << erro << std::endl;
    abort();
  }

  return configuracao.serial;
}

void MarcoCamera::ProcessarImagem(std::vector<Ente> &vecEnte)
{
  double tempoAbsoluto;

  std::vector<MarkerPontos> vecMarkerPontos;

  vecEnte.clear();
  tempoAbsoluto = Camera::CapturarImagem();

  extratorMarca->ExtrairMarcas(imgCamera,vecMarkerPontos,numLogger,configuracao.serial);

  std::vector<MarkerPontos>::iterator iteMarkerPontos = vecMarkerPontos.begin();
  double teta;
  for(;iteMarkerPontos<vecMarkerPontos.end();iteMarkerPontos++)
  {
    Camera::Localizar(iteMarkerPontos->centro);
    Camera::Localizar(iteMarkerPontos->verticeRef);
    Camera::Localizar(iteMarkerPontos->verticeSec);

    iteMarkerPontos->centro.setTempo(tempoAbsoluto);
    teta = iteMarkerPontos->verticeRef.CalcularAngulo(iteMarkerPontos->verticeSec);

    vecEnte.push_back( Ente(iteMarkerPontos->centro,teta) );
  }
}
