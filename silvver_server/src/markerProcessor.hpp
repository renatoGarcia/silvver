#ifndef MARKER_PROCESSOR_HPP
#define MARKER_PROCESSOR_HPP

#include <map>
#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include "silverTypes.hpp"
#include "processor.hpp"
#include <singleton.hpp>

class MarkerProcessor :  public Processor<silver::Identity<silver::Pose>,
                                          silver::Identity<silver::Pose> >,
                         public Singleton<MarkerProcessor>

{
public:

  void deliverPackage(const std::vector< silver::Identity<silver::Pose> > &pacote,
                      const unsigned id);

  // Calcula a configuraçãoo dos robôs, usando os dados atualmente
  // disponíveis no map armazenador.
  void localize(/*std::vector<silver::Ente> &vecRobos*/);

private:

  friend class Singleton<MarkerProcessor>;

  MarkerProcessor();

  boost::mutex mutexArmazenador;
};

#endif
