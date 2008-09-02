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
  boost::shared_ptr<Connection> connectionPtr;

  BOOST_FOREACH(Toutput output, vecResults)
  {
    connectionPtr.reset();

    *logFile << output.id << '\t' << output.x  << '\t'
             << output.y << '\t' << output.theta <<  std::endl;

    connectionPtr = this->clientsMap->findClient(output.id);

    if(connectionPtr)
    {
      connectionPtr->send((void*)&(output), sizeof(output));
    }
  }

}

#endif
