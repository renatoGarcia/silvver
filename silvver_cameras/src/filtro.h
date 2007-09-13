#include <iostream>
/**************************************************************************************
 Esta classe recebe uma IplImage em formato YUV(YCrCb,8bits sem sinal por camada em  
cada pixel), filtra as cores a serem localizadas, retornando 
em uma IplImagem em formato GRAY(8 bits por pixel e uma camada única) a localização
das cores. 
  Quando uma cor à ser localizada for identificada em um dado ponto na imagem YUV,
na imagem de retorno será atribuida ao ponto equivalente a este, uma tonalidade de  
cinza. Cada cor terá uma tonalidade, um valor inteiro entre 0 e 255, que a representa.
Nos pontos em que não forem detectadas nenhuma das cores buscadas será atribuido um
valor 0, correspondente à cor preta.
*************************************************************************************/
#ifndef FILTRO_H
#define FILTRO_H

#include <string>
#include <vector>
#include <cv.h>
#include <fstream>

using namespace std;

class Filtro
{
private:
    unsigned char yuvLUT[256][256][256];

    // Número de cores diferentes que estão sendo representadas.
    int numCores;

    void CarregarYUVLUT(ifstream *streamYUV);
public:

    Filtro();
     
    void Iniciar();
    void FiltrarYUVImagem(IplImage *imagemYUV,IplImage *imagemRetorno);   
};


#endif