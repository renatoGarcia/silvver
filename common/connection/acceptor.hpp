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

#include "channel.hpp"

/** Class to accept incoming connections.
 * The Type template argument must be an instantiation of AsioStream or any
 * derived class.
 */
template <class Type>
class Acceptor
{
public:
  /** Construct a new Acceptor, binding it to localEp.
   *
   * @param localEp The local endpont where to wait for incoming connections.
   */
  Acceptor(const typename Type::Endpoint& localEp);

  ~Acceptor();

  /** Wait for a connection request.
   * This method will block until a peer request a connection. The type of
   * created Channel will be dictated by peer.
   *
   * @param ioService The boost io_service to be assigned to Channel created.
   * @return A just created and alread connected Channel.
   */
  Channel* accept(boost::asio::io_service& ioService);

private:
  // As there aren't asynchronous operations, this io_service don't will be
  // run on a thread.
  static boost::asio::io_service ioService;

  // Since the type of resulting Channel is knew, this method will finalize the
  // connection acceptation.
  template <class TChannel>
  static
  Channel* createChannel(boost::asio::io_service& ioService, Type& tmpChannel);

  typename Type::Acceptor acceptor;
};

#endif /* _ACCEPTOR_HPP_ */
