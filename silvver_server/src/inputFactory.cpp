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

#include <boost/variant.hpp>

#include "input.ipp"
#include "ioConnection.ipp"
#include "silvverTypes.hpp"

#include "markerProcessor.hpp"

boost::shared_ptr<InputInterface>
InputFactory::createInput(const procOpt::AnyProcOpt& processorOpt,
                          const boost::shared_ptr<IoConnection>& connection)
{
  boost::shared_ptr<InputInterface> returnPtr;

  if (const procOpt::Marker* const marker =
      boost::get<procOpt::Marker>(&processorOpt))
  {
    returnPtr.reset(new Input<silvver::Identity<silvver::Pose> >
                    (connection, MarkerProcessor::instantiate()));
  }
  else
  {
    throw std::invalid_argument("Processor option 'get' not implemented in "
                                "InputFactory class.");
  }

  return returnPtr;
}
