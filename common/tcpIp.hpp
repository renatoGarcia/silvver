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

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/thread.hpp>
#include <string>
#include <vector>

#include "channel.hpp"
#include "channelTypes.hpp"

class TcpIp
  :public Channel
{
public:
  /** Get a hostname or ip address, and a service name or port number and
   * resolve to TcpIpEp.
   * @param address The hostname or ip address.
   * @param port The service name or port number.
   * @return A vector with all resolved TcpIpEp.  */
  static std::vector<TcpIpEp> resolve(const std::string& address,
                                      const std::string& port);

public:
  /** Construct a server side tcp/ip channel.
   * This constructor won't wait for income connection.
   * @param localEp The assigned local endpoint. */
  TcpIp(const TcpIpEp& localEp);

  /** Construct a client side tcp/ip channel.
   * This contructor will connect to server.
   * @param localEp The assigned local endpoint.
   * @param remoteEp The server endpoint.  */
  TcpIp(const TcpIpEp& localEp, const TcpIpEp& remoteEp);

  ~TcpIp() throw();

  virtual void close();

  /** Wait for and accept an income connection request.
   * Must be called only by server side. */
  void accept();

  virtual Endpoint getEndpoint() const;

  virtual void send(const std::string& data);

  virtual void receive(std::string& data);

private:
  static void runIoService();

  /// Length of TCP message header.
  static const unsigned HEADER_LENGTH = 8;

  static boost::asio::io_service ioService;

  static boost::scoped_ptr<boost::thread> th;

  static boost::once_flag onceFlag;

private:
  const TcpIpEp localEndpoint;

  boost::asio::ip::tcp::socket socket;
};

#endif /* _TCP_IP_HPP_ */
