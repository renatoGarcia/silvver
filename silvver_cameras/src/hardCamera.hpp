#ifndef HARDCAMERA_HPP
#define HARDCAMERA_HPP

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string>
#include "timer.hpp"

class HardCamera
{
protected:

  Timer timer;

public:

  //virtual ~HardCamera()=0;
  virtual void Iniciar(unsigned serial) = 0;
  virtual void Finalizar() = 0;
  virtual void SalvarImagem() = 0;
  virtual double CapturarImg(IplImage *imgRetorno) = 0;
};

#endif
