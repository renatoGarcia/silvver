#ifndef INPUT_FACTORY_HPP
#define INPUT_FACTORY_HPP

#include "inputInterface.hpp"
#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include "ioConnection.hpp"
#include <targetType.hpp>

class InputFactory
{
public:

  static boost::shared_ptr<InputInterface>
  createInput(TargetType targetType,
              boost::shared_ptr<IoConnection> connection);

private:

  InputFactory();
};

#endif
