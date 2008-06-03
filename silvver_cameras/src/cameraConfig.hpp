/**
 * @file   cameraConfig.hpp
 *
 * @brief  Estrutura e procedimento para representar e ler as
 *         informações sobre as câmeras que serão utilizadas.
 */
#ifndef CAMERACONFIG_HPP
#define CAMERACONFIG_HPP

#include <boost/array.hpp>
#include <stdexcept>
#include <vector>
#include <string>
#include "tinyXml/tinyxml.h"

typedef unsigned long long uint64;

class CameraConfig
{
public:

  enum enumModeloFisico{PGR,
                        DC1394,
                        PseudoCam
  }modeloFisico;

  enum enumModeloAbstrato{BLOB  = 103,
                          MARCO = 104
  }modeloAbstrato;

  float frameRate;
  uint64 serial;
  boost::array<unsigned,2> resolucao;

  boost::array<double,2> fc;     // Distância focal [pixels
  boost::array<double,2> cc;     // Coordenadas do ponto principal [pixels
  boost::array<double,5> kc;     // Coeficientes de distorção (radial e tangencial)
  double alpha_c;   // Coeficiente de inclinação (ângulo entre os eixos de pixels x e y)

  boost::array<double,9> H;

  std::string imagesPath;

  CameraConfig();

public:

  CameraConfig(const CameraConfig& camConfig);

};

class CameraConfigExtractor
{
public:

  class XmlLoad_error : std::runtime_error
  {
  public:
    XmlLoad_error(const std::string& whatArg):
      runtime_error(whatArg){};
  };

  std::vector<CameraConfig>
  lerDadosCameras(const std::string arquivoConfiguracao
                  )/*throw(std::invalid_argument,
                     boost::bad_lexical_cast,
                     TamanhoElemento)*/;

private:

  template <typename Tipo,int nItens>
  boost::array<Tipo, nItens> lerConteudoElemento(const TiXmlElement *elemento);

  template <typename Tipo>
  Tipo lerAtributoElemento(const TiXmlElement *elemento,
                           const std::string& nomeAtributo);

  std::vector<CameraConfig> vecCameraConfig;

};

#endif
