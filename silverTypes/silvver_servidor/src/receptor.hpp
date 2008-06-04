#ifndef RECEPTIONIST_HPP
#define RECEPTIONIST_HPP

#include <boost/thread/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include "entradas.hpp"
#include "saidas.hpp"
#include <sys/timeb.h>

extern bool verbose;

/** Cuida dos pedidos de conexão com o silvver-servidor.
 * Esta é a classe central de silvver servidor. Ela cuida de todos os pedidos de conexão,
 * cria um objeto Conexao permanente, e o redistribui para a classe apropriada.
 */
class Receptionist
{
public:
  Receptionist();

  ~Receptionist();

  /// Inicia a escuta por pedidos de conexão.
  void run();

  /// Encerra a escuta por pedidos de conexão.
  int FecharRecepcionista();

  /** Método que tratará de todos os pedidos de conexão ao silvver-servidor.
   * Este método ouvirá a porta dada por RECEPCIONISTA_PORTA, criará uma conexão permanente
   * para o novo cliente, e encaminhará a nova conexão para uma classe apropriada.
   */
  void operator()();

private:

  bool stopReceptionist;

  /// Tempo do sistema no instante inicial da execução do silvver-servidor, serve como referência para sincronizar as câmeras.
  struct timeb tempoInicial;

  /// Porta onde o método Recepcionista ouvirá os pedidos de conexão. O valor padrão é 12000.
  const unsigned RECEPCIONISTA_PORTA;

  /// Porta livre que será utilizada no próximo pedido de conexão.
  unsigned portaLivre;

  boost::scoped_ptr<Inputs> inputs;

  boost::scoped_ptr<Saidas> saidas;

  /// Thread onde onde será executado o método Recepcionista.
  boost::scoped_ptr<boost::thread> thRecepcionista;

  /// Retorna em segundos o tempo transcorrido desde a instanciação do receptor.
  double tempoDecorrido();

};

#endif
