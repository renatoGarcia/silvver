#ifndef COLOR_BLOB_INPUT_HPP
#define COLOR_BLOB_INPUT_HPP

#include "inputs.hpp"
#include <boost/shared_ptr.hpp>

class ColorBlobInput : public Inputs
{
public:

  ColorBlobInput(boost::shared_ptr<Connection> connection);

  virtual ~ColorBlobInput();

protected:

  virtual void operator()();

};

#endif

