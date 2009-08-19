#include "inputFactory.hpp"
#include "input.ipp"
#include "ioConnection.ipp"
#include <boost/lexical_cast.hpp>
#include "silverTypes.hpp"
#include "markerProcessor.hpp"

InputFactory::InputFactory()
{}

boost::shared_ptr<InputInterface>
InputFactory::createInput(TargetType targetType,
                          boost::shared_ptr<IoConnection> connection)
{
  boost::shared_ptr<InputInterface> returnPtr;

  switch(targetType.state)
  {
  case TargetType::ARTP_MARK:
    {
      returnPtr.reset(new Input<silver::Identity<silver::Pose> >
                      (connection, MarkerProcessor::instantiate()));
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
//       throw std::invalid_argument("Unknown inputType code: " +
//                                   boost::lexical_cast<std::string>(inputType));
    }
  }

  return returnPtr;
}
