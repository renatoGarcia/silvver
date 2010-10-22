/* Copyright 2009-2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#include "common/connection/channel.hpp"
#include "common/connection/exceptions.hpp"
#include "common/silvverTypes.hpp"
#include "inputInterface.hpp"
#include "outputMultiMap.hpp"
#include "processor.hpp"

/// Receive data sent by abstractCameras and deliver it to correct processor.
template <class Type>
class Input
  :public InputInterface
{
public:
  /** Input class constructor.
   *
   * @param channel A shared_ptr to a Channel object already connected
   *        with an abstractCamera.
   * @param processor A pointer to the correct processor.
   */
  Input(boost::shared_ptr<connection::Channel> channel,
        Processor<Type>* processor);

  ~Input();

private:
  void handleReceive(connection::error_code ec);

  /// Hold the last input received.
  silvver::CameraReading<Type> currentInput;

  /// The connection with the camera.
  boost::shared_ptr<connection::Channel> channel;

  Processor<Type>* processor;

  /// AbstractCamera Clients hearing for localizations before be processed.
  boost::shared_ptr<OutputMultiMap<silvver::AbstractCameraUid> > clientCameraMap;
};

#endif /* _INPUT_HPP_ */
