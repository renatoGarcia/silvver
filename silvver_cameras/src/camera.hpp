#include <iostream>
#include <highgui.h>
/*************************************************************************************
    Classe base para as diversas câmeras funcionais, como blobCamera e marcaCamera
**************************************************************************************/

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <cv.h>
#include "silvver_tipos.hpp"
#include "timer.hpp"
#include "hardCamera.hpp"
#include "hardCameras/modelos.hpp"

using namespace silvver;

class Camera
{
private:
  // Conta o tempo em que a camera ficou em funcionamento, usado para
  // calcular a taxa média de quadros por segundo.
  //Timer *timer;

  CameraConfig configuracao;

  // Número de imagens processadas, usado para calcular a taxa média
  // de quadros por segundo.
  unsigned quadros;
  float taxaDeQuadros;

  HardCamera *hardCamera; // Hardware que fará a captura das imagens.

protected:

  Camera(const CameraConfig& camConfig,double tempoInicial);

  ~Camera();

  void Iniciar();

  // Número serial que identifica univocamente a câmera utilizada
  //  unsigned cameraSerial;

  // Atualiza a imagem em "*imgCamera", e retorna o instante no tempo em que elas foram capturadas.
  // Este tempo é dado em milisegundos, e é calculado levando em conta o "tempoInicial".
  double CapturarImagem();

  // Converte a posição das coordenadas em pixels para as coordenadas do mundo
  void Localizar(Posicao &posicao);

public:

  IplImage *imgCamera;

  int TaxaQuadros();

};

#endif
