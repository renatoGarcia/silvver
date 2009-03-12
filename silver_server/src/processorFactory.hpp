#ifndef PROCESSOR_FACTORY_HPP
#define PROCESSOR_FACTORY_HPP

#include "processorInterface.hpp"
#include "silverTypes.hpp"
#include <boost/shared_ptr.hpp>
#include <stdexcept>

enum ProcessorType
{
  MARKER
};

template <typename Tinput>
class ProcessorFactory
{
public:

  static boost::shared_ptr< ProcessorInterface<Tinput> >
  createProcessor(ProcessorType processorType);

private:

  ProcessorFactory();
};

#endif
