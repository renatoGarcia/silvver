/**************************************************************************************
    Esta classe implementa uma c�mera que filma blobs. Ele tem como objetivo
    retornar um vector de CvConnectedComp com os blobs das cores encontradas.
**************************************************************************************/

#ifndef BLOBCAMERA_H
#define BLOBCAMERA_H

#include <vector>
#include <string>

#include "camera.hpp"
#include "filtro.hpp"
#include "blobExtractor.hpp"

using namespace std;
using namespace verlab;

class BlobCamera : public Camera
{
private:

    IplImage *imgTrabalho;

    Filtro *filtro;

    BlobExtractor *blobExtractor;

public:

    BlobCamera(const CameraConfig& camConfig,double tempoInicial);

    ~BlobCamera();

    int Iniciar();

    void ProcessarImagem(vector<Marca> &vecMarca);
};
#endif
