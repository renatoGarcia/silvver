#ifndef ARTP_MARK_INPUTS_HPP
#define ARTP_MARK_INPUTS_HPP

#include "inputs.hpp"
#include "markerProcessor.hpp"
#include <boost/shared_ptr.hpp>


class ARTPMarkInput : public Inputs
{
public:

  ARTPMarkInput(boost::shared_ptr<Connection> connection);

  virtual ~ARTPMarkInput();

protected:

  virtual void operator()();

private:

  MarkerProcessor *processor;

};

#endif
