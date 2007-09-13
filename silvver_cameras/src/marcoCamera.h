/**************************************************************************************
    Esta classe implementa uma c�mera que filma marcas. Ele tem como objetivo
    retornar um vector de Manchas com os blobs das cores encontradas.
**************************************************************************************/

#ifndef MARCOCAMERA_H
#define MARCOCAMERA_H

#include <vector>
#include <string>
#include "Camera.h"
#include "extratorMarca.h"

using namespace verlab;

class MarcoCamera : public Camera
{
private:

    ExtratorMarca *extratorMarca;

    int numLogger;

public:

    MarcoCamera(const CameraConfig& camConfig,double tempoInicial);

    ~MarcoCamera();

    int Iniciar();

    // Captura uma imagem e retorna
    void ProcessarImagem(vector<Ente> &vecEnte);

};

#endif
