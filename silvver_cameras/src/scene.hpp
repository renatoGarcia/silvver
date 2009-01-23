#ifndef SCENE_HPP
#define SCENE_HPP

#include <string>
#include <map>
#include <vector>

#include <boost/array.hpp>
#include <boost/variant.hpp>

namespace scene
{
  struct MatrixHomography
  {
    // Intrinsic parameters
    boost::array<double,2> fc;     // Distância focal [pixels
    boost::array<double,2> cc;     // Coordenadas do ponto principal [pixels
    boost::array<double,5> kc;     // Coeficientes de distorção (radial e tangencial)
    double alpha_c;   // Coeficiente de inclinação (ângulo entre os eixos de pixels x e y)

    // Extrinsic parameters
    boost::array<double,9> h;
  };

  struct LutHomography
  {
    boost::array<std::string,2> lut;
  };

  struct Camera
  {
    float frameRate;
    std::string hardware;
    std::string uid;
    boost::array<unsigned,2> resolution;

    /// Path to input images for pseudoCamera
    std::string imagesPath;

    boost::variant<MatrixHomography,LutHomography> homography;

    // If homography isn't MatrixHomography type, it throws boost::bad_get
    inline MatrixHomography& getMatrixHomography()
    {
      return boost::get<MatrixHomography>(this->homography);
    }

    // If homography isn't LutHomography type, it throws boost::bad_get
    inline LutHomography& getLutHomography()
    {
      return boost::get<LutHomography>(this->homography);
    }
  };

  struct Target
  {
    std::string targetDefineFile;

    unsigned uid;
  };

  struct Scene
  {
    std::vector<Camera> vecCamera;

    std::map< std::string, std::vector<Target> > targets;
  };
}
#endif /* SCENE_HPP */
