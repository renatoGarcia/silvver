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

#ifndef _TCP_IP_HPP_
#define _TCP_IP_HPP_

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <vector>

#include "asioStream.hpp"
#include "channelTypes.hpp"

namespace connection {

struct TcpIpSpec
{
  typedef boost::asio::ip::tcp AsioClass;
  static const ChannelType portableClassId = TCP_IP;
};

class TcpIp
  :public AsioStream<TcpIpSpec>
{
public:
  /** Get a hostname or ip address, and a service name or port number and
   * resolve to TcpIpEp.
   * @param address The hostname or ip address.
   * @param port The service name or port number.
   * @return A vector with all resolved TcpIpEp.  */
  static std::vector<boost::asio::ip::tcp::endpoint>
  resolve(const std::string& address,
          const std::string& port);

  /** Construct a server side tcp/ip channel.
   * This constructor won't wait for income connection.
   * @param localEp The assigned local endpoint. */
  TcpIp(boost::asio::io_service& ioService);

  /** Construct a client side tcp/ip channel.
   * This contructor will connect to server.
   * @param localEp The assigned local endpoint.
   * @param remoteEp The server endpoint.  */
  TcpIp(boost::asio::io_service& ioService, const Endpoint& localEp);

  virtual ~TcpIp() throw();
};

} // namespace connection

#endif /* _TCP_IP_HPP_ */
