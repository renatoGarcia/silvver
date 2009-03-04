#ifndef CLIENTS_MAP_HPP
#define CLIENTS_MAP_HPP

#include <map>
#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include "ioConnection.hpp"
#include <request.hpp>

class ClientsMap
{
public:

  static boost::shared_ptr<ClientsMap> instantiate();

  /** Add an client to client map.
   *
   * @param request An AddOutput object with the description of the client.
   * @param outputConnection An IoConnection already connected with the client.
   */
  void addOutput(AddOutput& request,
                 boost::shared_ptr<IoConnection> outputConnection);

  /** Retira o cliente com o id da lista de clientes.
   * Caso haja um cliente o id dado, ele será retirado da lista de clientes.
   *
   * @param id Identificador do cliente a ser retirado
   */
  void removeOutput(unsigned idTarget, unsigned remotePort);

  std::vector< boost::shared_ptr<IoConnection> > findClient(unsigned idTarget);

private:

  typedef std::multimap< unsigned, boost::shared_ptr<IoConnection> > TMultiMap;

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
