#ifndef PROCESSOR_INTERFACE_HPP
#define PROCESSOR_INTERFACE_HPP

#include <vector>

template <typename Tinput>
class ProcessorInterface
{
public:

  virtual void deliverPackage(std::vector<Tinput>& package, unsigned id)=0;

};

#endif
