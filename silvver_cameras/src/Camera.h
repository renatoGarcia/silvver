#include <iostream>
#include <highgui.h>
/**************************************************************************************
    Classe base para as diversas c�meras funcionais, como blobCamera e marcaCamera.
**************************************************************************************/

#ifndef CAMERA_H
#define CAMERA_H

#include <cv.h>
#include "tipos.h"
#include "timer.h"
#include "hardCamera.h"
#include "PGRCamera.h"
#include "pseudoCamera.h"

using namespace verlab;

// Informa��es de configura��o das c�meras usadas para a
// inicia��o das mesmas
struct CameraConfig
{
    enum enumModelo{PGR,PseudoCam}modelo;
    unsigned frequencia;
    unsigned serial;
    unsigned resolucao[2];

    double fc[2], cc[2], kc[5], alpha_c;
    double H[3][3];

    char* diretorio;
};

class Camera
{
private:
    
    double fc[2];     // Dist�ncia focal [pixels]
    double cc[2];     // Coordenadas do ponto principal [pixels]
    double kc[5];     // Coeficientes de distor��o (radial e tangencial)
    double alpha_c;   // Coeficiente de inclina��o (�ngulo entre os eixos de pixels x e y)

    double H[3][3];   // Matriz homogr�fica    

    // Conta o tempo em que a camera ficou em funcionamento, usado para
    // calcular a taxa m�dia de quadros por segundo.
    //Timer *timer;    

    // N�mero de imagens processadas, usado para calcular a taxa m�dia
    // de quadros por segundo.
    unsigned quadros;
    float taxaDeQuadros;

    HardCamera *hardCamera; // Hardware que far� a captura das imagens.

protected:

    Camera(const CameraConfig& camConfig,double tempoInicial);

    ~Camera();

    void Iniciar();

    // N�mero serial que identifica univocamente a c�mera utilizada
    unsigned cameraSerial;

    // Atualiza a imagem em "*imgCamera", e retorna o instante no tempo em que elas foram capturadas.
    // Este tempo � dado em milisegundos, e � calculado levando em conta o "tempoInicial".
    double CapturarImagem();    

    // Converte a posi��o das coordenadas em pixels para as coordenadas do mundo
    void Localizar(Posicao &posicao);

public:

    IplImage *imgCamera;

    int TaxaQuadros();

};

#endif
