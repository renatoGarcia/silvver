#include <iostream>
#include <opencv/highgui.h>

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <opencv/cv.h>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include "silvver_tipos.hpp"
#include "hardCamera.hpp"
#include "hardCameras/modelos.hpp"
#include "cameraConfig.hpp"

using namespace silvver;


// Classe base para as câmeras abstratas, como blobCamera e marcoCamera
class Camera : boost::noncopyable
{
private:

  //Parâmetros intrínsecos e extrínsecos da câmera física.
  double cc0, cc1;
  double fc0, fc1;
  double kc0, kc1, kc2, kc3, kc4;
  double H00, H01, H02,
         H10, H11, H12,
         H20, H21, H22;

  // Número de imagens processadas, usado para calcular a taxa média
  // de quadros por segundo.
  unsigned quadros;
  float taxaDeQuadros;

  HardCamera *hardCamera; // Hardware que fará a captura das imagens.

protected:

  CameraConfig configuracao;

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