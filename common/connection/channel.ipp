/* Copyright 2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#ifndef _CHANNEL_IPP_
#define _CHANNEL_IPP_

#include "channel.hpp"

#include <boost/bind.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/ref.hpp>

#include "serializator.hpp"

namespace connection {

template <class AcceptorType>
void
Channel::connect(const typename AcceptorType::Endpoint& acceptorEp)
{
  boost::asio::io_service ioService;

  AcceptorType tmpChannel(ioService);
  tmpChannel.getSocket().connect(acceptorEp);

  std::string strChannelType;
  serialize(strChannelType, this->channelType);
  tmpChannel.write(strChannelType);

  std::string finalAcceptorEp;
  tmpChannel.read(finalAcceptorEp);
  effectConnection(finalAcceptorEp);
}

template <class T>
void
Channel::send(const T& t)
{
  std::string data;
  serialize(data, t);
  this->write(data);
}

template <class T>
void
Channel::receive(T& t)
{
  std::string data;
  this->read(data);
  deserialize(t, data);
}

template <class T>
void
Channel::asyncReceive(T& t, ReceiveHandler handler)
{
  boost::shared_ptr<std::string> data(new std::string);
  this->asyncRead(data, boost::bind(&Channel::readHandler<T>, this,
                                    data, boost::ref(t), handler, _1, _2));
}

template <class T>
void
Channel::readHandler(boost::shared_ptr<std::string> data,
                     T& t,
                     ReceiveHandler handler,
                     const boost::system::error_code& ec,
                     std::size_t bytes_transferred)
{
  error_code error(error_code::success);

  if ((ec == boost::asio::error::broken_pipe) ||
      (ec == boost::asio::error::connection_aborted))
  {
    error = error_code::broken_connection;
    if (!this->closeHandler.empty())
    {
      this->closeHandler();
    }
  }
  else if(ec)
  {
    error = error_code::connection_error;
  }
  else
  {
    try
    {
      deserialize(t, *data);
    }
    catch (const data_error& e)
    {
      error = error_code::data_error;
    }
  }

  handler(error);
}

} // namespace connection

#endif /* _CHANNEL_IPP_ */
