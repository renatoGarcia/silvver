/**************************************************************************************
    Esta classe implementa uma c�mera que filma marcas. Ele tem como objetivo
    retornar um vector de Manchas com os blobs das cores encontradas.
**************************************************************************************/

#ifndef MARCOCAMERA_HPP
#define MARCOCAMERA_HPP

#include <vector>
#include <string>
#include "Camera.hpp"
#include "extratorMarca.hpp"
#include "cameraConfig.hpp"

using namespace silvver;

class MarcoCamera : public Camera
{
private: 

  ExtratorMarca *extratorMarca;

  int numLogger;

public:

  MarcoCamera(const CameraConfig& camConfig,double tempoInicial);

  ~MarcoCamera();

  int Iniciar();

  // Captura uma imagem e retorna
  void ProcessarImagem(vector<Ente> &vecEnte);

};

#endif
