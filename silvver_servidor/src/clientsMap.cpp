#include "clientsMap.hpp"
// #include <iostream>

boost::shared_ptr<ClientsMap> ClientsMap::singleInstance;
boost::mutex ClientsMap::instantiatingMutex;

boost::shared_ptr<ClientsMap>
ClientsMap::instantiate()
{
  boost::mutex::scoped_lock lock(ClientsMap::instantiatingMutex);
  if(!ClientsMap::singleInstance)
  {
    ClientsMap::singleInstance.reset(new ClientsMap);
  }

  return ClientsMap::singleInstance;
}

ClientsMap::ClientsMap()
{}

void
ClientsMap::addOutput(boost::shared_ptr<Connection> outputConnection)
{
  char msg[3];
  char OK[3] = "OK";
  int clientId;

  outputConnection->receive(msg,sizeof(msg)); //Deverá receber como resposta a mensagem "OK".
//   std::cout << "Confirma conexao: " << msg << std::endl;

  outputConnection->receive( &clientId,sizeof(int) );
//   std::cout << "ID: " << clientId << std::endl;

  {
    boost::mutex::scoped_lock lock(this->accessMap);

    this->client[clientId] = outputConnection;

    this->client.find(clientId)->second->send(OK,sizeof(OK));
  }
}

void
ClientsMap::removeOutput(int id)
{
  boost::mutex::scoped_lock lock(this->accessMap);

  this->client.erase(this->client.find(id));
}
