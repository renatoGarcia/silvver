#include <iostream>
#include <fstream>
/**************************************************************************************/
#ifndef PGR_H
#define PGR_H

#include <pgrflycapture.h>
#include <pgrflycaptureplus.h>
#include <sstream>
#include <string>
#include "../hardCamera.h"

using namespace std;

#define NUMERO_BUFFERS 4

class PGR: public HardCamera
{
private:

  FlyCaptureContext contexto;

  FlyCaptureImagePlus fcImagemRaw;
  FlyCaptureImage fcImagemBGR;

  // Buffer onde as imagens capturadas serão armazenadas
  unsigned char* buffers[NUMERO_BUFFERS];

  // Caminho do diretório no qual as imagens serão salvas
  string diretorio;

  // Número da imagem atual que será salva, será o nome da mesma.
  int numeroImagem;

  // Número serial da camera PGR.
  int serial;

  FlyCaptureFrameRate frequencia;

public:
  PGR(unsigned frequencia,char* diretorio,double tempoInicial);

  ~PGR();

  virtual void Iniciar(unsigned serial);

  virtual void Finalizar();

  virtual double CapturarImg(IplImage *imgRetorno);

  // Grava a última imagem da câmera no disco
  virtual void SalvarImagem();

};

#endif
