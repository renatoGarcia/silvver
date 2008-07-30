#include "inputFactory.hpp"
#include <boost/lexical_cast.hpp>
#include "artpMarkInput.hpp"
#include "colorBlobInput.hpp"

InputFactory::InputFactory()
{}

boost::shared_ptr<Inputs>
InputFactory::createInput(InputTypes inputType,
                          boost::shared_ptr<Connection> connection)
{
  boost::shared_ptr<Inputs> returnPtr;

  switch(inputType)
  {
  case ARTP_MARK:
    {
      returnPtr.reset(new ARTPMarkInput(connection));
      break;
    }
  case COLOR_BLOB:
    {
      returnPtr.reset(new ColorBlobInput(connection));
      break;
    }
  default:
    {
      throw std::invalid_argument("Unknown inputType code: " +
                                  boost::lexical_cast<std::string>(inputType));
    }
  }

  return returnPtr;
}
