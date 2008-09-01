#ifndef OUTPUTS_HPP
#define OUTPUTS_HPP

#include <map>
#include "silverTypes.hpp"
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include "connection.hpp"
#include <fstream>

/** Implementa o envio das poses desejadas a cada cliente.
 * A classe Saidas mantem um registro de todos os clientes,
 * e a cada novo vetor de estado recebido reenvia para cada cliente a sua nova pose
 */
class Outputs
{
private:

  static boost::shared_ptr<Outputs> singleInstance;
  static boost::mutex instantiatingMutex;
  Outputs();

  boost::mutex sendResultsMutex;

  /** Socket de comunicação com os clientes que receberão a sua própria pose.
   * A chave int é o id de interesse.
   */
  std::map< int,Connection* > client;

  /// Registrará todas as informações obtidas das cameras
  boost::scoped_ptr<std::ofstream> logFile;

public:

  static boost::shared_ptr<Outputs> instantiate();

  /** Recebe uma conexão com um cliente e cria uma nova saída.
   * Recebe um objeto da classe Conexão contendo as informações necessárias para a comunicação com o cliente, e finaliza a criação de uma nova saída.
   *
   * @param outputConnection Um objeto da classe Conexão que contenha o endereço IP do novo cliente e a porta utilizada para a cominicação.
   */
  void addOutput(Connection *outputConnection);

  /** Retira o cliente com o id da lista de clientes.
   * Caso haja um cliente o id dado, ele será retirado da lista de clientes.
   *
   * @param id Identificador do cliente a ser retirado
   */
  void removeOutput(int id);

  /** Recebe a pose de todos os robôs localizados, processa
   * e envia essa informação para os clientes.
   * @param vecRobo Vetor com todos os marcos localizados. Representados por um objeto da classe Ente.
   */
  void sendResults(const std::vector<silver::Ente> &vecRobo);
};

#endif
