/**************************************************************************************
    Esta classe cuida do estabelecimento de novas conex�es. Ela lidar� tanto com
   os clientes(c�meras), quanto com as saidas, que s�o conex�es com os programas
   que far�o uso dos resultados obtidos por este programa.
**************************************************************************************/
#ifndef RECEPTOR_H
#define RECEPTOR_H

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "entradas.h"
#include "saidas.h"
#include <sys/timeb.h>

using namespace boost;

class Receptor
{
private:

  static auto_ptr<Receptor> instanciaUnica;
  static mutex mutexInstanciacao;
  Receptor();

  // Tempo referência para o inicio da execução do programa, serve para sincronizar as câmeras
  struct timeb tempoInicial;

  // Porta onde o recepcionista estará
  const unsigned RECEPCIONISTA_PORTA;

  // Porta que para que o cliente se conecte
  unsigned portaLivre;

  Entradas *entradas;

  Saidas *saidas;

  // Thread que cuidará da aceitação de novas blobCâmeras
  thread *thRecepcionista;

  // Método que tratará de todos os pedidos de conexão ao programa, seja de câmeras
  // ou de robôs.
  static void Recepcionista(Receptor *objeto);

  // Retorna em segundos o tempo passado desde a instanciação do receptor.
  double tempoDecorrido();

public:

  static Receptor* Instanciar();

  void CriarRecepcionista();

  int FecharRecepcionista();
};

#endif
