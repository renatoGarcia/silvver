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

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <vector>

#include "clientType.hpp"

/// Wrapper all network communication with silvver-server.
class Connection
{
public:

  Connection(const std::string& serverIp, unsigned receptionistPort);

  ~Connection();

  void connect(ClientType clientType, unsigned targetId);

  void disconnect(ClientType clientType, unsigned targetId);

  /** Asynchronously read a value of type T.
   * @param t Reference to the variable where the read value will be returned.
   * @param handler Function with signature "void handler()" which to be called
   *                when the operation completes. */
  template <typename T, typename Handler>
  inline void
  asyncRead(T& t, Handler handler)
  {
    this->inputSocket.async_receive
      (boost::asio::buffer(this->inboundData),
       boost::bind(&Connection::readData<T, Handler>,
                   this,
                   boost::asio::placeholders::error,
                   boost::asio::placeholders::bytes_transferred,
                   boost::ref(t),
                   boost::make_tuple(handler)));
  }

private:

  friend class boost::thread;

  /// Length of TCP message header.
  static const unsigned HEADER_LENGTH = 8;

  /// Max length of UPD messages.
  static const unsigned UPD_MAX_LENGTH = 8192;

  static boost::asio::io_service ioService;

  static boost::scoped_ptr<boost::thread> th;

  static boost::mutex runMutex;
  static bool ioServiceRunning;

  static void runIoService();

  boost::asio::ip::tcp::socket receptionistSocket;

  const boost::asio::ip::tcp::endpoint receptionistEP;

  boost::asio::ip::udp::socket inputSocket;

  std::vector<char> inboundData;

  template <typename T>
  void writeToReceptionist(const T& t);

  template <typename T>
  void readFromReceptionist(T& t);

  template <typename T, typename Handler>
  void readData(const boost::system::error_code& e,
                std::size_t bytes_transferred,
                T& t,
                boost::tuple<Handler> handler);
};

#endif // CONNECTION_HPP
