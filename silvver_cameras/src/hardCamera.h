#ifndef HARDCAMERA_H
#define HARDCAMERA_H

#include <cv.h>
#include <highgui.h>
#include <string>
#include "timer.h"

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
