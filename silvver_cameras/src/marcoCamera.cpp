#include "marcoCamera.hpp"

MarcoCamera::MarcoCamera(const CameraConfig& camConfig,double tempoInicial)
 :Camera(camConfig,tempoInicial)
{
  extratorMarca = new ExtratorMarca(camConfig.resolucao[0],camConfig.resolucao[1]);
}

MarcoCamera::~MarcoCamera()
{
  delete extratorMarca;
}

int MarcoCamera::Iniciar()
{
  try
  {
    Camera::Iniciar();
    this->numLogger = extratorMarca->Iniciar();
  }
  catch(string erro)
  {
    cout << "Erro: " << erro << endl;
    abort();
  }

  return configuracao.serial;
}

void MarcoCamera::ProcessarImagem(vector<Ente> &vecEnte)
{
  double tempoAbsoluto;

  vector<MarkerPontos> vecMarkerPontos;

  vecEnte.clear();
  tempoAbsoluto = Camera::CapturarImagem();

  extratorMarca->ExtrairMarcas(imgCamera,vecMarkerPontos,numLogger,configuracao.serial);

  vector<MarkerPontos>::iterator iteMarkerPontos = vecMarkerPontos.begin();
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
