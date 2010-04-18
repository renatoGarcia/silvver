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

#ifndef _INPUT_HPP_
#define _INPUT_HPP_

#include <boost/shared_ptr.hpp>
#include <vector>

#include "inputInterface.hpp"
#include "ioConnection.hpp"
#include "outputMap.hpp"
#include "processorInterface.hpp"

/// Receive the data sent by cameras and send it to correct processor class.
template <class Type>
class Input: public InputInterface
{
public:
  /** Input class constructor.
   * @param connection A shared_ptr to an IoConnection already connected
   *                   with a camera.
   * @param processor A shared_ptr to the correct processor.  */
  Input(boost::shared_ptr<IoConnection> connection,
        boost::shared_ptr<ProcessorInterface<Type> > processor);

  ~Input();

private:
  void handleReceive();

  /// Hold the last inputs received.
  std::vector<Type> currentInputs;

  /// The connection with the camera.
  boost::shared_ptr<IoConnection> connection;

  /// The local port where hearing for incoming data from cameras.
  unsigned connectionPort;

  boost::shared_ptr<ProcessorInterface<Type> > processor;

  /// Clients hearing for localizations before be processed.
  boost::shared_ptr<OutputMap<CLIENT_RAW> > outputRawMap;
};

#endif /* _INPUT_HPP_ */
