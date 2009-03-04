#include "clientsMap.hpp"
#include <iostream>
#include "ioConnection.ipp"

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
ClientsMap::addOutput(AddOutput& request,
                      boost::shared_ptr<IoConnection> outputConnection)
{
//   std::cout << "ID: " << clientId << std::endl;
  boost::mutex::scoped_lock lock(this->accessMap);

  this->client.insert(std::pair< unsigned, boost::shared_ptr<IoConnection> >
                      (request.targetId, outputConnection));
}

void
ClientsMap::removeOutput(unsigned idTarget, unsigned remotePort)
{
  boost::mutex::scoped_lock lock(this->accessMap);

  std::pair< TMultiMap::iterator, TMultiMap::iterator >
    range(this->client.equal_range(idTarget));

  for(TMultiMap::iterator it = range.first;
      it != range.second;
      ++it)
  {
    if(it->second->getRemotePort() == remotePort)
    {
      this->client.erase(it);
    }
  }
}

std::vector< boost::shared_ptr<IoConnection> >
ClientsMap::findClient(unsigned idTarget)
{
  boost::mutex::scoped_lock lock(this->accessMap);

  std::pair< TMultiMap::iterator, TMultiMap::iterator >
    range(this->client.equal_range(idTarget));

  std::vector< boost::shared_ptr<IoConnection> > vecConnection;
  for(TMultiMap::iterator it = range.first;
      it != range.second;
      ++it)
  {
    vecConnection.push_back(it->second);
  }

  return vecConnection;
}
