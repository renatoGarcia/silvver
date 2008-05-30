#include "controlador.hpp"


// --------------------- MarcoCameraControlador ----------------------------//

MarcoCameraControlador::MarcoCameraControlador(CameraConfig cameraConfig,
					       unsigned porta,
					       std::string ipServidor)
{
  this->cameraConfig = cameraConfig;
  this->porta = porta;
  this->ipServidor = ipServidor;
  this->termina = false;
}

