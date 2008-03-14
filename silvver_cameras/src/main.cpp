#include <iostream>
#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include "gerenciador.hpp"
#include "cameraConfig.hpp"
#include "cmdline.h"

// Mutex para controlar a escrita na saida padrão
boost::mutex mutexCout;

int main(int argc, char **argv)
{
  gengetopt_args_info args_info;
  if (cmdline_parser (argc, argv, &args_info) != 0)
    return 1;

  std::string ipServidor(args_info.ip_servidor_arg);
  int portaRecepcionista = args_info.porta_recepcionista_arg;

  std::vector<CameraConfig> vecCameraConfig;
  CameraConfig::LerDadosCameras(vecCameraConfig);

  Gerenciador gerenciador(portaRecepcionista,ipServidor,vecCameraConfig);

  gerenciador.RodarGerenciador();

  char ch = getchar(); //Espera o usuário teclar enter para terminar.

  std::cout << "Terminando..." << std::endl;

  gerenciador.PararGerenciador();

  std::cout << "Fim do Programa" << std::endl;

  return 0;
}
