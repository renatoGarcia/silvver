#ifndef MARCOCAMERA_HPP
#define MARCOCAMERA_HPP

#include <vector>
#include <string>
#include <boost/scoped_ptr.hpp>
#include "camera.hpp"
#include "extratorMarca.hpp"
#include "cameraConfig.hpp"

using namespace silvver;

/// Implementa uma câmera abstrata que filma marcos.
class MarcoCamera : public Camera
{
private:

  boost::scoped_ptr<ExtratorMarca> extratorMarca;

  int numLogger;

public:

  MarcoCamera(const CameraConfig& camConfig,double tempoInicial);

  int Iniciar();

  /** Captura uma imagem extrai a localização dos marcos.
   * @param vecEnte Vetor onde são retornadas as poses e identificações dos marcos.
   */
  void ProcessarImagem(vector<Ente> &vecEnte);

};

#endif
