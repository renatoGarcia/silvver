/**************************************************************************************
    Esta classe representa o conjunto de clientes(blobCameras) conectados a este
  servidor. Para cada cliente haver� uma thread espec�fica para trat�-lo
**************************************************************************************/

#ifndef ENTRADAS_H
#define ENTRADAS_H

#include <vector>
#include <string>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include "conexao.h"
#include "tipos.h"
#include "saidas.h"
#include "blobTratador.h"
#include "marcaTratador.h"

using namespace std;
using namespace boost;
using namespace verlab;

enum TipoDado
{
  CORES  = 103,
  MARCAS = 104
};

class Entradas
{
private:

  static auto_ptr<Entradas> instanciaUnica;
  static mutex mutexInstanciacao;
  Entradas();

  // Cada item representará uma thread que irá receber
  // informações de uma câmera.
  vector<thread*> threadCliente;

  // Controlará as informações recebidas de uma blobCamera
  static void ClienteBlobCam(Entradas *objeto, Conexao *conexao);

  // Controlará as informações recebidas de uma marcaCamera
  static void ClienteMarcaCam(Entradas *objeto, Conexao *conexao);

  Saidas *saidas;

  string tipoDado2string(TipoDado td);

public:

  static Entradas* Instanciar();

  void AdicionarEntrada(Conexao *conexao);
};

#endif
