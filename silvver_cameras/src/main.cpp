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

  std::string serverIP(args_info.ip_servidor_arg);
  int portaRecepcionista = args_info.porta_recepcionista_arg;
  std::string xmlConfigCameras = args_info.cameras_config_arg;

  std::vector<CameraConfig> vecCameraConfig;
  CameraConfigExtractor camConfExtractor;

  vecCameraConfig = camConfExtractor.lerDadosCameras(xmlConfigCameras);

  Gerenciador gerenciador(portaRecepcionista,serverIP,vecCameraConfig);

  gerenciador.RodarGerenciador();

  std::string command;
  while(true)
  {
    std::cout << ">";
    std::getline(std::cin, command, '\n');

    if(command == "q")
    {
      break;
    }
  }

  gerenciador.PararGerenciador();

  std::cout << "Fim do Programa" << std::endl;

  return 0;
}
