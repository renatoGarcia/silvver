#include "clientsMap.hpp"
#include <iostream>

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
  std::cout << "Confirma conexao: " << msg << std::endl;

  outputConnection->receive( &clientId,sizeof(int) );
  std::cout << "ID: " << clientId << std::endl;

  {
    boost::mutex::scoped_lock lock(this->accessMap);

    TMultiMap::iterator it =
      this->client.insert(std::pair< unsigned,boost::shared_ptr<Connection> >
                          (clientId, outputConnection));

    it->second->send(OK,sizeof(OK));
  }

}

void
ClientsMap::removeOutput(unsigned idTarget, unsigned connectionPort)
{
  boost::mutex::scoped_lock lock(this->accessMap);

  std::pair< TMultiMap::iterator, TMultiMap::iterator >
    range(this->client.equal_range(idTarget));

  for(TMultiMap::iterator it = range.first;
      it != range.second;
      ++it)
  {
    if(it->second->getPort() == connectionPort)
    {
      this->client.erase(it);
    }
  }
}

std::vector< boost::shared_ptr<Connection> >
ClientsMap::findClient(unsigned idTarget)
{
  boost::mutex::scoped_lock lock(this->accessMap);

  std::pair< TMultiMap::iterator, TMultiMap::iterator >
    range(this->client.equal_range(idTarget));

  std::vector< boost::shared_ptr<Connection> > vecConnection;
  for(TMultiMap::iterator it = range.first;
      it != range.second;
      ++it)
  {
    vecConnection.push_back(it->second);
  }

  return vecConnection;
}
