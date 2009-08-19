#ifndef PROCESSOR_IMPL_HPP
#define PROCESSOR_IMPL_HPP

#include "processor.hpp"
#include <boost/foreach.hpp>
#include "ioConnection.ipp"

template <typename Tinput, typename Toutput>
Processor<Tinput,Toutput>::Processor()
{
  this->clientsMap = ClientsMap::instantiate();
}

template <typename Tinput, typename Toutput>
void
Processor<Tinput,Toutput>::sendToOutputs(const std::vector<Toutput> &localizations) const
{
  std::vector< boost::shared_ptr<IoConnection> > vecConnection;
  boost::shared_ptr<IoConnection> connectionPtr;

  BOOST_FOREACH(Toutput output, localizations)
  {
    vecConnection = this->clientsMap->findClient(output.uid);

    BOOST_FOREACH(connectionPtr, vecConnection)
    {
      connectionPtr->send(output);
    }
  }

}

#endif
