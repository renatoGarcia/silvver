/**
 * @file   cameraConfig.hpp
 *
 * @brief  Estrutura e procedimento para representar e ler as
 *         informações sobre as câmeras que serão utilizadas.
 */
#ifndef CAMERACONFIG_HPP
#define CAMERACONFIG_HPP

#include <vector>
#include <string>

using namespace std;

class CameraConfig
{
public:

  enum enumFisico{PGR,PseudoCam}modeloFisico;
  enum enumAbstrato{BLOB=103,MARCO=104}modeloAbstrato;
  unsigned frequencia;
  unsigned serial;
  unsigned resolucao[2];

  double fc[2];     // Distância focal [pixels
  double cc[2];     // Coordenadas do ponto principal [pixels
  double kc[5];     // Coeficientes de distorção (radial e tangencial)
  double alpha_c;   // Coeficiente de inclinação (ângulo entre os eixos de pixels x e y)

  double H[3][3];

  string diretorio;

  CameraConfig();

public:

  static void LerDadosCameras(vector<CameraConfig> &vecCameraConfig);

  CameraConfig(const CameraConfig& camConfig);

};

#endif
