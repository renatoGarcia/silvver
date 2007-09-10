/**************************************************************************************
    Esta classe implementa uma c�mera que filma marcas. Ele tem como objetivo
    retornar um vector de Manchas com os blobs das cores encontradas.
**************************************************************************************/

#ifndef MARCACAM_H
#define MARCACAM_H

#include <vector>
#include <string>
#include "Camera.h"
#include "extratorMarca.h"

using namespace verlab;

class MarcaCamera : public Camera
{
private:

    ExtratorMarca *extratorMarca;

    int numLogger;

public:

    MarcaCamera(const CameraConfig& camConfig,double tempoInicial);

    ~MarcaCamera();

    int Iniciar();

    // Captura uma imagem e retorna
    void ProcessarImagem(vector<Ente> &vecEnte);

};

#endif