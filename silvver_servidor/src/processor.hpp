#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "processorInterface.hpp"
#include "outputs.hpp"
#include "silverTypes.hpp"
#include <boost/shared_ptr.hpp>
#include <vector>

template <typename Tinput, typename Toutput>
class Processor : public ProcessorInterface<Tinput>
{
public:

  virtual void deliverPackage(silver::Package<Tinput> &package, unsigned id)=0;

protected:

  Processor();

  typedef std::map<unsigned,std::vector<Tinput> > TMapa;

  // Armazenara os ultimos entes obtidos de cada marcaCamera, onde
  // a chave e um identificador destas.
  TMapa armazenador;

  void sendToOutputs(const std::vector<Toutput> &localizations) const;

private:

  boost::shared_ptr< Outputs<Toutput> > outputs;
};


#endif
