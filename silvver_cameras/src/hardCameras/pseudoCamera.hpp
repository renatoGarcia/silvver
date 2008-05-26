#ifndef PSEUDOCAMERA_HPP
#define PSEUDOCAMERA_HPP

#include "../hardCamera.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/mutex.hpp>

class PseudoCamera : public HardCamera
{
private:

  std::string endImagem;

  int imagemAtual;

  int totalImagens;

  // Tempo que pseudoCamera ficará ociosa para simular uma dada frequência
  int atraso;

public:

  PseudoCamera(int totalImagens,unsigned frequencia,const char* diretorio);

  ~PseudoCamera();

  virtual void initialize();

  virtual void saveFrame();

  virtual void captureFrame(IplImage *iplImage);

};

#endif
