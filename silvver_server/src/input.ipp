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

#ifndef _INPUT_IPP_
#define _INPUT_IPP_

#include "input.hpp"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include "common/connection/channel.ipp"
#include "common/serializations.hpp"
#include "log.hpp"

template <typename Type>
Input<Type>::Input(boost::shared_ptr<connection::Channel> channel,
                   boost::shared_ptr< ProcessorInterface<Type> > processor)
  :InputInterface()
  ,currentInput()
  ,channel(channel)
  ,processor(processor)
  ,clientCameraMap(OutputMultiMap<silvver::AbstractCameraUid>::instantiate())
{
  this->channel->asyncReceive(this->currentInput,
                              boost::bind(&Input<Type>::handleReceive,
                                          this, _1));
}

template <typename Type>
Input<Type>::~Input()
{}

template <typename Type>
void
Input<Type>::handleReceive(connection::error_code ec)
{
  message(MessageLogLevel::DEBUG)
    << ts_output::lock
    << "Received " << this->currentInput.localizations.size()
    << " targets from camera "
    << this->currentInput.camUid << std::endl
    << ts_output::unlock;

  std::vector<boost::shared_ptr<connection::Channel> > vecChannels;
  boost::shared_ptr<connection::Channel> channelPtr;

  // Get all camera clients hearing for a given camera.
  this->clientCameraMap->findOutputs(this->currentInput.camUid,
                                     vecChannels);
  BOOST_FOREACH(channelPtr, vecChannels)
  {
    channelPtr->send(this->currentInput);
  }

  this->processor->deliverPackage(this->currentInput);

  this->channel->asyncReceive(this->currentInput,
                              boost::bind(&Input<Type>::handleReceive,
                                          this, _1));
}

#endif /* _INPUT_IPP_ */
