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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _ACCEPTOR_IPP_
#define _ACCEPTOR_IPP_

#include "acceptor.hpp"

#include <boost/ref.hpp>
#include <memory>
#include <string>

#include "channelTypes.hpp"
#include "exceptions.hpp"
#include "serializator.hpp"

#include "tcpIp.hpp"
#include "unixSocket.hpp"

namespace connection {

template <class Type>
Acceptor<Type>::Acceptor(boost::asio::io_service& ioService,
                         const typename Type::Endpoint& localEp)
  :acceptor(ioService, localEp)
{}

template <class Type>
Acceptor<Type>::~Acceptor()
{}

template <class Type>
Channel*
Acceptor<Type>::accept(boost::asio::io_service& ioService)
{
  Type tmpChannel(ioService);
  this->acceptor.accept(tmpChannel.getSocket());

  std::string data;
  tmpChannel.read(data);
  ChannelType channelType;
  deserialize(channelType, data);

  if (channelType == TCP_IP)
  {
    return createChannel<TcpIp>(ioService, tmpChannel);
  }
  else if (channelType == UNIX_SOCKET)
  {
    return createChannel<UnixSocket>(ioService, tmpChannel);
  }
  else
  {
    throw data_error("Unknown channel type");
  }
}

template <class Type>
void
Acceptor<Type>::asyncAccept(boost::asio::io_service& ioService,
                            AcceptHandler handler)
{
  boost::shared_ptr<Type> tmpChannel(new Type(ioService));
  this->acceptor.async_accept(tmpChannel->getSocket(),
                              boost::bind(&Acceptor<Type>::receiveChannelType,
                                          tmpChannel, boost::ref(ioService),
                                          handler, _1));
}

template <class Type>
void
Acceptor<Type>::receiveChannelType(boost::shared_ptr<Type> tmpChannel,
                                   boost::asio::io_service& ioService,
                                   AcceptHandler handler,
                                   const boost::system::error_code& ec)
{
  std::string data;
  tmpChannel->read(data);
  ChannelType channelType;
  deserialize(channelType, data);

  if (channelType == TCP_IP)
  {
    handler(createChannel<TcpIp>(ioService, *tmpChannel),
            error_code::success);
  }
  else if (channelType == UNIX_SOCKET)
  {
    handler(createChannel<UnixSocket>(ioService, *tmpChannel),
            error_code::success);
  }
  else
  {
    handler(NULL, error_code::data_error);
  }
}

template <class Type>
template <class TChannel>
Channel*
Acceptor<Type>::createChannel(boost::asio::io_service& ioService,
                              Type& tmpChannel)
{
  std::auto_ptr<TChannel> channel(new TChannel(ioService));
  typename TChannel::Acceptor acceptor(ioService,
                                       typename TChannel::Endpoint());

  std::string data;
  serialize(data, acceptor.local_endpoint());
  tmpChannel.write(data);

  acceptor.accept(channel->getSocket());

  return channel.release();
}

} // namespace connection

#endif /* _ACCEPTOR_IPP_ */
