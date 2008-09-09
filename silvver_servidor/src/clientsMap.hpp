#ifndef CLIENTS_MAP_HPP
#define CLIENTS_MAP_HPP

#include <map>
#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include "connection.hpp"

class ClientsMap
{
public:

  static boost::shared_ptr<ClientsMap> instantiate();

  /** Recebe uma conexão com um cliente e cria uma nova saída.
   * Recebe um objeto da classe Conexão contendo as informações necessárias para a comunicação com o cliente, e finaliza a criação de uma nova saída.
   *
   * @param outputConnection Um objeto da classe Conexão que contenha o endereço IP do novo cliente e a porta utilizada para a cominicação.
   */
  void addOutput(boost::shared_ptr<Connection> outputConnection);

  /** Retira o cliente com o id da lista de clientes.
   * Caso haja um cliente o id dado, ele será retirado da lista de clientes.
   *
   * @param id Identificador do cliente a ser retirado
   */
  void removeOutput(unsigned idTarget, unsigned connectionPort);

  std::vector< boost::shared_ptr<Connection> > findClient(unsigned idTarget);

private:

  typedef std::multimap< unsigned,boost::shared_ptr<Connection> > TMultiMap;

  static boost::shared_ptr<ClientsMap> singleInstance;
  static boost::mutex instantiatingMutex;

  boost::mutex accessMap;

  ClientsMap();

  /**
   * A chave int é o id do alvo de interesse.
   */
  TMultiMap client;

};

#endif
