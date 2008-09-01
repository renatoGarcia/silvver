#ifndef OUTPUTS_IMPL_HPP
#define OUTPUTS_IMPL_HPP

#include "outputs.hpp"
#include <boost/foreach.hpp>

template <typename Toutput>
Outputs<Toutput>::Outputs()
{
  this->logFile.reset(new std::ofstream("log.txt"));

  this->clientsMap = ClientsMap::instantiate();
}

template <typename Toutput>
void
Outputs<Toutput>::sendResults(const std::vector<Toutput> &vecResults)
{
  boost::mutex::scoped_lock lock(clientsMap->accessMap);

  BOOST_FOREACH(Toutput output, vecResults)
  {
    *logFile << output.id << '\t' << output.x  << '\t'
             << output.y << '\t' << output.theta <<  std::endl;

    //Caso exista um cliente com o mesmo id do Ente apontado por ite envia a pose para ele.
    if(clientsMap->client.find(output.id) != clientsMap->client.end())
    {
      clientsMap->client.find(output.id)->second->send((void*)&(output),
                                                       sizeof(output) );
    }
  }

}

#endif
