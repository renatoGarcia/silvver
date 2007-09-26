#include <iostream>
#include <string.h>
#include <vector>
#include "controlador.hpp"
#include "cameraConfig.hpp"

using namespace std;

// Mutex para controlar a escrita na saida padrão
boost::mutex mutexCout;

int main(int argc, char **argv)
{

  char ip[16]="127.0.0.1";
  int portaRecepcionista = 12000;

  if(argc>1)
  {
    strncpy(ip,argv[1],16);
  }
  if(argc>2)
  {
    portaRecepcionista = atoi(argv[2]);
  }

  TipoDado tipoDado = /*CORES*/MARCAS;

  //-------------------------------------------------------------//

  vector<CameraConfig> vecCameraConfig;
  LerDadosCameras(vecCameraConfig);

  Controlador *controlador;
  controlador = new Controlador(portaRecepcionista,ip,tipoDado,vecCameraConfig);

  controlador->RodarControlador();

  char ch = getchar(); //Espera o usuário teclar enter para terminar.

  cout << "Terminando..." << endl;

  controlador->PararControlador();
  delete controlador;

  cout << "Fim do Programa" << endl;

  return 0;
}
