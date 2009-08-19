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

#ifndef _IO_CONNECTION_HPP_
#define _IO_CONNECTION_HPP_

#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>

/** Class to connect to silver-cameras and silver-clients.
 * Wrapper a UDP/IP socket and silver message protocol.
 */
class IoConnection
{
public:

  /**
   * Construct an ioConnection and connect it to the given remote endpoint.
   * @param ioService An io_service running in some thread.
   * @param remoteIpAddress IP address of remote endpoint.
   * @param remotePort Port number of remote endpoint.
   */
  IoConnection(const std::string& remoteIpAddress,
               const unsigned short remotePort);

  ~IoConnection();

  inline void close()
  {
    this->socket.shutdown(boost::asio::ip::udp::socket::shutdown_both);
    this->socket.close();
  }

  inline unsigned short getLocalPort() const
  {
    return this->socket.local_endpoint().port();
  }

  inline unsigned short getRemotePort() const
  {
    return this->socket.remote_endpoint().port();
  }

  template <typename T>
  void send(const T& t);

  /**
   * Asynchronously receive a value of type T.
   * @param t Reference to the variable where the read value will be returned.
   * @param handler Function with signature "void handler()" which to be called
   *                when the operation completes.
   */
  template <typename T, typename Handler>
  inline void
  asyncReceive(T& t, Handler handler)
  {
    this->socket.async_receive(boost::asio::buffer(this->inboundData),
                               boost::bind(&IoConnection::readData<T, Handler>,
                                           this,
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred,
                                           boost::ref(t),
                                           boost::make_tuple(handler)));
  }

private:

  static boost::asio::io_service ioService;
  static boost::asio::io_service::work work;

  static boost::scoped_ptr<boost::thread> th;

  static boost::once_flag onceFlag;

  static void runIoService();

  static const unsigned UPD_MAX_LENGTH = 8192;

  boost::asio::ip::udp::socket socket;

  std::vector<char> inboundData;

  template <typename T, typename Handler>
  void
  readData(const boost::system::error_code& e,
           std::size_t bytes_transferred,
           T& t,
           boost::tuple<Handler> handler);
};

#endif /*  _IO_CONNECTION_HPP_ */
