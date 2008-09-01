#include "inputFactory.hpp"
#include "input_impl.hpp"
#include <boost/lexical_cast.hpp>
#include "silverTypes.hpp"
#include "processorFactory.hpp"

InputFactory::InputFactory()
{}

boost::shared_ptr<InputInterface>
InputFactory::createInput(InputType inputType,
                            boost::shared_ptr<Connection> connection)
{
  boost::shared_ptr<InputInterface> returnPtr;

  switch(inputType)
  {
  case ARTP_MARK:
    {
      ProcessorType t = MARKER;
      returnPtr.reset(new Input<silver::Ente>(connection,
                                             t));
      break;
    }
//   case COLOR_BLOB:
//     {
//       returnPtr.reset(new Input<silver::Blob>(connection
//                                               ProcessorType::MARKER));
//       break;
//     }
  default:
    {
      throw std::invalid_argument("Unknown inputType code: " +
                                  boost::lexical_cast<std::string>(inputType));
    }
  }

  return returnPtr;
}
