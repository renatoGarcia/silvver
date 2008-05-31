#ifndef SAIDAS_HPP
#define SAIDAS_HPP

#include <map>
#include "tipos.hpp"
#include <boost/thread/mutex.hpp>
#include "connection.hpp"
#include <fstream>

using namespace std;
using namespace boost;
using namespace verlab;

/** Implementa o envio das poses desejadas a cada cliente.
 * A classe Saidas mantem um registro de todos os clientes,
 * e a cada novo vetor de estado recebido reenvia para cada cliente a sua nova pose
 */
class Saidas
{
private:

  static auto_ptr<Saidas> instanciaUnica;
  static mutex mutexInstanciacao;
  Saidas();

  mutex mutexReceberEstado;

  /** Socket de comunicação com os clientes que receberão a sua própria pose.
   * A chave int é o id de interesse.
   */
  map< int,Conexao* > clientes;

  /// Registrará todas as informações obtidas das cameras
  ofstream *arqLog;

public:

  static Saidas* Instanciar();

  /** Recebe uma conexão com um cliente e cria uma nova saída.
   * Recebe um objeto da classe Conexão contendo as informações necessárias para a comunicação com o cliente, e finaliza a criação de uma nova saída.
   *
   * @param conexaoNovoCliente Um objeto da classe Conexão que contenha o endereço IP do novo cliente e a porta utilizada para a cominicação.
   */
  void AdicionarSaida(Conexao *conexaoNovoCliente);

  /** Retira o cliente com o id da lista de clientes.
   * Caso haja um cliente o id dado, ele será retirado da lista de clientes.
   *
   * @param id Identificador do cliente a ser retirado
   */
  void RetirarSaida(int id);

  /** Recebe a pose de todos os robôs localizados, processa
   * e envia essa informação para os clientes.
   * @param vecRobo Vetor com todos os marcos localizados. Representados por um objeto da classe Ente.
   */
  void ReceberEstado(const vector<Ente> &vecRobo);
};

#endif
