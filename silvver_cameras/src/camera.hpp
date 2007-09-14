#include <iostream>
#include <highgui.h>
/*************************************************************************************
    Classe base para as diversas câmeras funcionais, como blobCamera e marcaCamera
**************************************************************************************/

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <cv.h>
#include "tipos.hpp"
#include "timer.hpp"
#include "hardCamera.hpp"

using namespace verlab;

// Informações de configuração das câmeras usadas para a
// iniciação das mesmas
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
    double fc[2];     // Distância focal [pixels
    double cc[2];     // Coordenadas do ponto principal [pixels
    double kc[5];     // Coeficientes de distorção (radial e tangencial
    double alpha_c;   // Coeficiente de inclinação (ângulo entre os eixos de pixels x e y)

    double H[3][3];   // Matriz homográfica

    // Conta o tempo em que a camera ficou em funcionamento, usado para
    // calcular a taxa média de quadros por segundo.
    //Timer *timer;

    // Número de imagens processadas, usado para calcular a taxa média
    // de quadros por segundo.
    unsigned quadros;
    float taxaDeQuadros;

    HardCamera *hardCamera; // Hardware que fará a captura das imagens.

protected:

    Camera(const CameraConfig& camConfig,double tempoInicial);

    ~Camera();

    void Iniciar();

    // Número serial que identifica univocamente a câmera utilizada
    unsigned cameraSerial;

    // Atualiza a imagem em "*imgCamera", e retorna o instante no tempo em que elas foram capturadas.
    // Este tempo é dado em milisegundos, e é calculado levando em conta o "tempoInicial".
    double CapturarImagem();

    // Converte a posição das coordenadas em pixels para as coordenadas do mundo
    void Localizar(Posicao &posicao);

public:

    IplImage *imgCamera;

    int TaxaQuadros();

};

#endif
