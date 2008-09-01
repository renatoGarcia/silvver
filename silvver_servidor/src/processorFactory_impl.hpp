#ifndef PROCESSOR_FACTORY_IMPL_HPP
#define PROCESSOR_FACTORY_IMPL_HPP

#include "processorFactory.hpp"
#include <boost/lexical_cast.hpp>

#include "markerProcessor.hpp"

template <typename Tinput>
ProcessorFactory<Tinput>::ProcessorFactory()
{}

template <typename Tinput>
boost::shared_ptr< ProcessorInterface<Tinput> >
ProcessorFactory<Tinput>::createProcessor(ProcessorType processorType)
{
  boost::shared_ptr< ProcessorInterface<Tinput> > returnPtr;

  switch(processorType)
  {
  case MARKER:
    {
      returnPtr = MarkerProcessor::instantiate();
      break;
    }
  default:
    {
      throw std::invalid_argument("Unknown processorType code: " +
                                  boost::lexical_cast<std::string>(processorType));
    }
  }

  return returnPtr;
}

#endif
