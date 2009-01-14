#ifndef SCENE_HPP
#define SCENE_HPP

#include <string>
#include <boost/array.hpp>
#include <map>
#include <vector>

typedef unsigned long long uint64;

struct CameraConfig
{
  float frameRate;
  std::string hardware;
  uint64 uid;
  boost::array<unsigned,2> resolution;

  /// Path to input images for pseudoCamera
  std::string imagesPath;

  //----------------------- Intrinsic parameters -----------------------//
  boost::array<double,2> fc;     // Distância focal [pixels
  boost::array<double,2> cc;     // Coordenadas do ponto principal [pixels
  boost::array<double,5> kc;     // Coeficientes de distorção (radial e tangencial)
  double alpha_c;   // Coeficiente de inclinação (ângulo entre os eixos de pixels x e y)

  // Extrinsic parameters
  boost::array<double,9> H;

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

#endif /* SCENE_HPP */
