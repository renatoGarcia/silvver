#ifndef PROCESSOR_INTERFACE_HPP
#define PROCESSOR_INTERFACE_HPP

#include "silverTypes.hpp"

template <typename Tinput>
class ProcessorInterface
{
public:

  virtual void deliverPackage(silver::Package<Tinput> &package, unsigned id)=0;

};

#endif
