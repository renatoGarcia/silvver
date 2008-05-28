#ifndef RECEPTOR_H
#define RECEPTOR_H

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "entradas.h"
#include "saidas.h"
#include <sys/timeb.h>

extern bool verbose;

using namespace boost;

/** Cuida dos pedidos de conexão com o silvver-servidor.
 * Esta é a classe central de silvver servidor. Ela cuida de todos os pedidos de conexão,
 * cria um objeto Conexao permanente, e o redistribui para a classe apropriada.
 */
class Receptor
{
private:

  static auto_ptr<Receptor> instanciaUnica;
  static mutex mutexInstanciacao;
  Receptor();

  /// Tempo do sistema no instante inicial da execução do silvver-servidor, serve como referência para sincronizar as câmeras.
  struct timeb tempoInicial;

  /// Porta onde o método Recepcionista ouvirá os pedidos de conexão. O valor padrão é 12000.
  const unsigned RECEPCIONISTA_PORTA;

  /// Porta livre que será utilizada no próximo pedido de conexão.
  unsigned portaLivre;

  Entradas *entradas;

  Saidas *saidas;

  /// Thread onde onde será executado o método Recepcionista.
  thread *thRecepcionista;

  /** Método que tratará de todos os pedidos de conexão ao silvver-servidor.
   * Este método ouvirá a porta dada por RECEPCIONISTA_PORTA, criará uma conexão permanente
   * para o novo cliente, e encaminhará a nova conexão para uma classe apropriada.
   *
   * @param objeto Apontador para a instância da classe Receptor, ponteiro para this.
   */
  static void Recepcionista(Receptor *objeto);

  /// Retorna em segundos o tempo transcorrido desde a instanciação do receptor.
  double tempoDecorrido();

public:

  /// Esta é uma classe solitária, este método deve ser usado ao invés do construtor.
  static Receptor* Instanciar();

  /// Inicia a escuta por pedidos de conexão.
  void CriarRecepcionista();

  /// Encerra a escuta por pedidos de conexão.
  int FecharRecepcionista();
};

#endif
