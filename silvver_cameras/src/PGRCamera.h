#include <iostream>
#include <fstream>
/**************************************************************************************/
#ifndef PGRCAMERA_H
#define PGRCAMERA_H

#include <pgrflycapture.h>
#include <pgrflycaptureplus.h>
#include <sstream>
#include <string>
#include "hardCamera.h"

using namespace std;

#define NUMERO_BUFFERS 4

class PGRCamera : public HardCamera
{
private:   

    FlyCaptureContext contexto;

    FlyCaptureImagePlus fcImagemRaw;
    FlyCaptureImage fcImagemBGR;    

    // Buffer onde as imagens capturadas ser�o armazenadas
    unsigned char* buffers[NUMERO_BUFFERS];

    // Caminho do diret�rio no qual as imagens ser�o salvas
    string diretorio;

    // N�mero da imagem atual que ser� salva, ser� o nome da mesma.
    int numeroImagem;

    // N�mero serial da PGRCamera;
    int serial;

    FlyCaptureFrameRate frequencia;

public:
    PGRCamera(unsigned frequencia,char* diretorio,double tempoInicial);

    ~PGRCamera();

    virtual void Iniciar(unsigned serial);

    virtual void Finalizar();

    virtual double CapturarImg(IplImage *imgRetorno);

    // Grava a �ltima imagem da c�mera no disco
    virtual void SalvarImagem();
 
};

#endif
