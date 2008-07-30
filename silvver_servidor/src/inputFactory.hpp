#ifndef INPUT_FACTORY_HPP
#define INPUT_FACTORY_HPP

#include "inputs.hpp"
#include <boost/shared_ptr.hpp>
#include <stdexcept>

enum InputTypes
{
  COLOR_BLOB = 103,
  ARTP_MARK  = 104
};


class InputFactory
{
public:

  static boost::shared_ptr<Inputs>
  createInput(InputTypes inputType, boost::shared_ptr<Connection> connection);

private:

  InputFactory();

};

#endif
