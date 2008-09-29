/**
 * @file   xmlParser.hpp
 *
 * @brief  Estrutura e procedimento para representar e ler as
 *         informações sobre as câmeras que serão utilizadas.
 */
#ifndef XMLPARSER_HPP
#define XMLPARSER_HPP

#include <boost/array.hpp>
#include <stdexcept>
#include <vector>
#include <map>
#include <string>
#include "tinyXml/tinyxml.h"

typedef unsigned long long uint64;

struct CameraConfig
{
  float frameRate;
  std::string hardware;
  uint64 uid;
  boost::array<unsigned,2> resolution;

  boost::array<double,2> fc;     // Distância focal [pixels
  boost::array<double,2> cc;     // Coordenadas do ponto principal [pixels
  boost::array<double,5> kc;     // Coeficientes de distorção (radial e tangencial)
  double alpha_c;   // Coeficiente de inclinação (ângulo entre os eixos de pixels x e y)

  boost::array<double,9> H;

  std::string imagesPath;

      boost::array<std::string,2> lut;

      bool useLut;
};

struct TargetConfig
{
  std::string targetDefineFile;

  unsigned uid;
};

class Scene
{
public:

  std::vector<CameraConfig> vecCameraConfig;

  std::map< std::string, std::vector<TargetConfig> > targets;
};

class XmlParser
{
public:

  class XmlLoad_error : std::runtime_error
  {
  public:
    XmlLoad_error(const std::string& whatArg):
      runtime_error(whatArg){};
  };

  Scene parseFile(std::string xmlFile);

private:

  template <typename Type,int nItens>
  boost::array<Type,nItens> readElementText(const TiXmlElement *element);

  template <typename Type>
  Type readAttribute(const TiXmlElement *element,
                     std::string attributeName);
};

#endif
