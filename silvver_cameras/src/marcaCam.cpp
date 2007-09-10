#include "marcaCam.h"

MarcaCamera::MarcaCamera(const CameraConfig& camConfig,double tempoInicial)
:Camera(camConfig,tempoInicial)
{
    extratorMarca = new ExtratorMarca(camConfig.resolucao[0],camConfig.resolucao[1]);
}

MarcaCamera::~MarcaCamera()
{
    delete extratorMarca;
}

int MarcaCamera::Iniciar()
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

    return cameraSerial;
}

void MarcaCamera::ProcessarImagem(vector<Ente> &vecEnte)
{
  double tempoAbsoluto;

  vector<MarkerPontos> vecMarkerPontos;

  vecEnte.clear();
  tempoAbsoluto = Camera::CapturarImagem();

  extratorMarca->ExtrairMarcas(imgCamera,vecMarkerPontos,numLogger,this->cameraSerial);

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
