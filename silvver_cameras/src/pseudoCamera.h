#include <iostream>
/**************************************************************************************

*************************************************************************************/

#ifndef PSEUDOCAMERA_H
#define PSEUDOCAMERA_H

#include "hardCamera.h"
#include <string>
#include <sstream>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/mutex.hpp>

using namespace std;

class PseudoCamera : public HardCamera
{
private:

    string endImagem;

    int imagemAtual;

    int totalImagens;

    // Tempo que pseudoCamera ficar� ociosa para simular uma dada frequ�ncia
    int atraso;

public:

    PseudoCamera(int totalImagens,unsigned frequencia,char* diretorio);

    ~PseudoCamera();

    virtual void Iniciar(unsigned serial);

    virtual void Finalizar();

    virtual void SalvarImagem();

    virtual double CapturarImg(IplImage *imgRetorno);//TODO: implementar o retorno com o tempo da imagem

};

#endif