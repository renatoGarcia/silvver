  /**************************************************************************************
    Esta classe implementa um controlador que centraliza o trabalho das v�rias
  blobC�meras.
**************************************************************************************/

#ifndef CONTROLADOR_H
#define CONTROLADOR_H

#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include "tinyXml/tinyxml.h"
#include "Camera.h"
#include "BlobCamera.h"
#include "marcaCam.h"
#include "conexao.h"
#include "tipos.h"
#include "timer.h"

using namespace std;
using namespace boost;
using namespace verlab;

enum TipoDado
{
    CORES  = 103,
    MARCAS = 104
};

class Controlador
{
private:

    // Sincroniza a criação e iniciação de uma nova blobCamera.
    static mutex mutexInicarCamera;

    // Cont�m o caminho e os nomes dos arquivos de texto que definem as cores
    vector<string> vectorCores;

    // Conjunto das estruturas de configura��o de todas as c�meras
    vector<CameraConfig> vecCameraConfig;

    // Threads em que os m�todos controladores das c�meras ser�o executados
    vector<thread*> thCamera;

    // Conex�o com o m�todo que cuidar� da aceita��o de novas c�meras
    /*static*/ Conexao *conexaoRecepcionista;

    // C�digo que representa o tipo de dado que ser� enviado para o receptor
    const TipoDado DADO;

    // Porta � qual enviar a primeira mensagem, a fim de se
    // descobrir em qual porta a BlobCamera deve se conectar
    int portaReceptor;

    // Ip do receptor
    char *ipReceptor;

    // Ser� true quando for para encerrar as blobCameras
    static bool termina;

    // Controlar� uma blobCamera. � executado em uma thread separada.
    static void BlobCam(const CameraConfig &cameraConfig,vector<string> vecCores,int porta,char *ip);

    // Controlar� uma marcaCamera. � executado em uma thread separada.
    static void MarcaCam(const CameraConfig &cameraConfig,int porta,char *ip,double tempoInicial);

    // Prepara um conex�o para um nova blobC�mera no servidor
    void ConectarCamera(const CameraConfig &cameraConfig);

public:

    Controlador(int porta,char *ip,TipoDado codigoDado,vector<CameraConfig> &vecCameraConfig);

    ~Controlador();

    // Para cada par n�mero Serial/matriz H passados pelo arquivo
    // cameras.txt cria uma thread que rodar� o m�todo BlobCam()
    void RodarControlador();

    // Fecha todas as blobCameras, e prepara o controlador para terminar
    void PararControlador();
};

#endif
