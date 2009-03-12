#ifndef PROCESSOR_IMPL_HPP
#define PROCESSOR_IMPL_HPP

#include "processor.hpp"
#include "outputs_impl.hpp"

template <typename Tinput, typename Toutput>
Processor<Tinput,Toutput>::Processor()
{
  this->outputs.reset(new Outputs<Toutput>());
}

template <typename Tinput, typename Toutput>
void
Processor<Tinput,Toutput>::sendToOutputs(const std::vector<Toutput> &localizations) const
{
  this->outputs->sendResults(localizations);
}

#endif
