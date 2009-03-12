#ifndef HOMOGRAPHY_HPP
#define HOMOGRAPHY_HPP

#include <fstream>

#include <boost/array.hpp>

#include "silverTypes.hpp"
#include "scene.hpp"

class Homography
{
public:

  virtual ~Homography()
  {}

  virtual void transform2word(silver::Position& position) const = 0;
};

//--------------------------- MATRIX ---------------------------//

class Matrix : public Homography
{
public:

  Matrix(const scene::MatrixHomography& matrix);

  void transform2word(silver::Position& position) const;

private:

  //Parâmetros intrínsecos e extrínsecos da câmera física.
  double cc0, cc1;
  double fc0, fc1;
  double kc0, kc1, kc2, kc3, kc4;
  double H00, H01, H02,
         H10, H11, H12,
         H20, H21, H22;
};

//-------------------------- LUT --------------------------//

class Lut : public Homography
{
public:

  Lut(const scene::LutHomography& lut,
      const boost::array<unsigned, 2>& resolution);

  ~Lut();

  void transform2word(silver::Position& position) const;

private:

  double **lutX;
  double **lutY;

  unsigned width;
  unsigned height;
};

#endif /* HOMOGRAPHY_HPP */
