#ifndef INPUT_FACTORY_HPP
#define INPUT_FACTORY_HPP

#include "inputInterface.hpp"
#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include "connection.hpp"

enum InputType
{
  COLOR_BLOB = 103,
  ARTP_MARK  = 104
};

class InputFactory
{
public:

  static boost::shared_ptr<InputInterface>
  createInput(InputType inputType, boost::shared_ptr<Connection> connection);

private:

  InputFactory();
};

#endif
