#ifndef OUTPUTS_IMPL_HPP
#define OUTPUTS_IMPL_HPP

#include "outputs.hpp"
#include <boost/foreach.hpp>
#include "ioConnection.ipp"

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
  std::vector< boost::shared_ptr<IoConnection> > vecConnection;
  boost::shared_ptr<IoConnection> connectionPtr;

  BOOST_FOREACH(Toutput output, vecResults)
  {
    *logFile << output.id << '\t' << output.x  << '\t'
             << output.y << '\t' << output.theta <<  std::endl;

    vecConnection = this->clientsMap->findClient(output.id);

    BOOST_FOREACH(connectionPtr, vecConnection)
    {
      connectionPtr->send(output);
    }
  }
}

#endif
