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

#ifndef _ACCEPTOR_HPP_
#define _ACCEPTOR_HPP_

#include <boost/asio/io_service.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>

#include "channel.hpp"

namespace connection {

/** Class to accept incoming connections.
 * The Type template argument must be an instantiation of AsioStream or any
 * derived class.
 */
template <class Type>
class Acceptor
{
public:
  typedef boost::function<void (Channel*, error_code)> AcceptHandler;

  /** Construct a new Acceptor, binding it to localEp.
   *
   * @param ioService A boost io_service with threads used in async operations.
   * @param localEp The local endpont where to wait for incoming connections.
   */
  Acceptor(boost::asio::io_service& ioService,
           const typename Type::Endpoint& localEp);

  ~Acceptor();

  /** Wait for a connection request.
   * This method will block until a peer request a connection. The type of
   * created Channel will be dictated by peer.
   *
   * @param ioService The boost io_service to be assigned to Channel created.
   * @return A just created and alread connected Channel.
   */
  Channel* accept(boost::asio::io_service& ioService);

  /** Asynchronously wait for a connection request.
   * This method will return immediately, and will call the handler when a
   * connection is made. A pointer to created Channel will be passed to handler
   * as argument, its concrete type will be dictated by peer.
   *
   * @param ioService The boost io_service to be assigned to Channel created.
   * @param hander A functional with signature
   *        void handler(Channel*, error_code).
   */
  void asyncAccept(boost::asio::io_service& ioService, AcceptHandler handler);

private:
  static
  void receiveChannelType(boost::shared_ptr<Type> tmpChannel,
                          boost::asio::io_service& ioService,
                          AcceptHandler handler,
                          const boost::system::error_code& ec);

  // Since the type of resulting Channel is knew, this method will finalize the
  // connection acceptation.
  template <class TChannel>
  static
  Channel* createChannel(boost::asio::io_service& ioService, Type& tmpChannel);

  typename Type::Acceptor acceptor;
};

} // namespace connection

#endif /* _ACCEPTOR_HPP_ */
