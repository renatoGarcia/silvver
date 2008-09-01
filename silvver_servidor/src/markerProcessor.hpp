#ifndef MARKER_PROCESSOR_HPP
#define MARKER_PROCESSOR_HPP

#include <map>
#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include "silverTypes.hpp"
#include "processor.hpp"

class MarkerProcessor : public Processor<silver::Ente,silver::Ente>
{
public:

  static boost::shared_ptr<MarkerProcessor> instantiate();

  void deliverPackage(silver::Package<silver::Ente> &pacote, unsigned id);

  // Calcula a configuraçãoo dos robôs, usando os dados atualmente
  // disponíveis no map armazenador.
  void localize(/*std::vector<silver::Ente> &vecRobos*/);

private:

  static boost::shared_ptr<MarkerProcessor> singleInstance;
  static boost::mutex instantiatingMutex;
  MarkerProcessor();

  boost::mutex mutexArmazenador;

};

#endif
