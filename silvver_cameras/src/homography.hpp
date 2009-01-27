#ifndef HOMOGRAPHY_HPP
#define HOMOGRAPHY_HPP

#include "silverTypes.hpp"
#include "scene.hpp"
#include <fstream>

class Homography
{
public:

  virtual ~Homography()
  {}

  virtual void transform2word(silver::Position& position) const = 0;

protected:

  Homography()
  {}

private:

};

class Matrix : public Homography
{
public:

  inline Matrix(const scene::MatrixHomography& matrix)
    :Homography()
  {
    this->cc0 = matrix.cc.at(0);
    this->cc1 = matrix.cc.at(1);

    this->fc0 = matrix.fc.at(0);
    this->fc1 = matrix.fc.at(1);

    this->kc0 = matrix.kc.at(0);
    this->kc1 = matrix.kc.at(1);
    this->kc2 = matrix.kc.at(2);
    this->kc3 = matrix.kc.at(3);
    this->kc4 = matrix.kc.at(4);

    this->H00 = matrix.h.at(0);
    this->H01 = matrix.h.at(1);
    this->H02 = matrix.h.at(2);
    this->H10 = matrix.h.at(3);
    this->H11 = matrix.h.at(4);
    this->H12 = matrix.h.at(5);
    this->H20 = matrix.h.at(6);
    this->H21 = matrix.h.at(7);
    this->H22 = matrix.h.at(8);
  }

  inline void transform2word(silver::Position& position) const
  {
    //--------------------------------- Parâmetros Intrínsecos
    double xc[2];
    xc[0] = position.x;
    xc[1] = position.y;
    double x[2];
    double x_distort[2];
    x_distort[0] = (xc[0] - cc0) / fc0;
    x_distort[1] = (xc[1] - cc1) / fc1;

    double delta_x[2];
    double k_radial, r2;
    x[0] = x_distort[0];
    x[1] = x_distort[1];
    for (int i = 0; i < 20; i++)
    {
      r2 = x[0]*x[0] + x[1]*x[1];
      k_radial = 1.0 + kc0 * r2 + kc1 * pow(r2,2) + kc4 * pow(r2,3);
      delta_x[0] = 2 * kc2 * x[0] * x[1] + kc3 * (r2 + 2 * pow(x[0],2));
      delta_x[1] = kc2 * (r2 + 2 * pow(x[1],2)) + 2 * kc3 * x[0] * x[1];
      x[0] = (x_distort[0] - delta_x[0]) / k_radial;
      x[1] = (x_distort[1] - delta_x[1]) / k_radial;
    }
    position.x = x[0] * fc0 + cc0;
    position.y = x[1] * fc1 + cc1;

    //-------------------------------- Parâmetros Extrínsecos

    double escala,xPixel,yPixel,xMundo,yMundo;

    xPixel = position.x;
    yPixel = position.y;

    escala = H20 * xPixel + H21 * yPixel + H22;
    xMundo = (H00 * xPixel +
              H01 * yPixel +
              H02) / escala;
    yMundo = (H10 * xPixel +
              H11 * yPixel +
              H12) / escala;

    position.x = xMundo;
    position.y = yMundo;
  }

private:

  //Parâmetros intrínsecos e extrínsecos da câmera física.
  double cc0, cc1;
  double fc0, fc1;
  double kc0, kc1, kc2, kc3, kc4;
  double H00, H01, H02,
    H10, H11, H12,
    H20, H21, H22;

};

class Lut : public Homography
{
public:
  Lut(const scene::LutHomography& lut)
    :Homography()
  {
    std::ifstream lutXFile(lut.lut.at(0).c_str());
    std::ifstream lutYFile(lut.lut.at(1).c_str());
//     this->width = cameraConfig.resolution.at(0); AQUI!!!
//     this->height = cameraConfig.resolution.at(1);

    lutX = new double*[this->height + 1];
    for (int i = 0; i < this->height + 1; i++)
      lutX[i] = new double[this->width + 1];

    lutY = new double*[this->height + 1];
    for (int i = 0; i < this->height + 1; i++)
      lutY[i] = new double[this->width + 1];

    for(int i = 0; i < this->height; i++)
      for (int j = 0; j < this->width; j++)
        lutXFile >> lutX[i][j];

    for(int i = 0; i < this->height; i++)
      for (int j = 0; j < this->width ; j++)
        lutYFile >> lutY[i][j];

    // Preenchendo a "borda extra"
    for(int i = 0; i < this->height; i++)
    {
      lutX[i][this->width] = lutX[i][this->width-1];
      lutY[i][this->width] = lutY[i][this->width-1];
    }
    for(int j = 0; j < this->width; j++)
    {
      lutX[this->height][j] = lutX[this->height-1][j];
      lutY[this->height][j] = lutY[this->height-1][j];
    }
  }

  ~Lut()
  {
    for (int i = 0; i < this->height; i++)
    {
      delete lutX[i];
      delete lutY[i];
    }
    delete lutX;
    delete lutY;
  }

  inline void transform2word(silver::Position& position) const
  {
    double x = position.x;
    double y = position.y;
    double xFactor = x - floor(x);
    double yFactor = y - floor(y);
    int iX = floor(x);
    int iY = floor(y);

    if (iY >=0 && iX >= 0 && iY < height && iX < width)
    {
      position.x = lutX[iY][iX] * (1-xFactor)*(1-yFactor) +
                   lutX[iY + 1][iX] * (1-xFactor)*(yFactor) +
                   lutX[iY][iX + 1] * (xFactor)*(1-yFactor) +
                   lutX[iY + 1][iX + 1] * (xFactor)*(yFactor);

      position.y = lutY[iY][iX] * (1-xFactor)*(1-yFactor) +
                   lutY[iY + 1][iX] * (1-xFactor)*(yFactor) +
                   lutY[iY][iX + 1] * (xFactor)*(1-yFactor) +
                   lutY[iY + 1][iX + 1] * (xFactor)*(yFactor);
    }
    else
    {
      std::cerr << "Cuidado! Tentativa de acessar posicao invalida"
                << std::endl;
    }
  }

private:

  double **lutX;
  double **lutY;

  int width;
  int height;
};


#endif /* HOMOGRAPHY_HPP */
