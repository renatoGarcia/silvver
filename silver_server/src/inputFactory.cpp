/* Copyright 2009 Renato Florentino Garcia <fgar.renato@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "inputFactory.hpp"

#include <boost/lexical_cast.hpp>

#include "input.ipp"
#include "ioConnection.ipp"
#include "silverTypes.hpp"

#include "markerProcessor.hpp"

InputFactory::InputFactory()
{}

boost::shared_ptr<InputInterface>
InputFactory::createInput(const TargetType targetType,
                          const boost::shared_ptr<IoConnection>& connection)
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
