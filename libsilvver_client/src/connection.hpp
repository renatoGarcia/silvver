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

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <vector>

#include "silvverTypes.hpp"

/// Wrapper all network communication with silvver-server.
class Connection
{
public:

  // Can throw silvver::connection_error
  Connection(const std::string& serverName,
             const std::string& receptionistPort,
             const silvver::TargetUid& targetUid);

  // Can throw silvver::connection_error
  Connection(const std::string& serverName,
             const std::string& receptionistPort,
             const silvver::AbstractCameraUid& cameraUid);

  ~Connection();

  template <typename T>
  void read(T& t);

  /** Asynchronously read a value of type T.
   * @param t Reference to the variable where the read value will be returned.
   * @param handler Function with signature "void handler()" which to be called
   *                when the operation completes. */
  template <typename T, typename Handler>
  inline
  void
  asyncRead(T& t, Handler handler)
  {
    boost::asio::async_read(this->socket,
                            boost::asio::buffer(this->inboundHeader),
                            boost::bind(&Connection::readHeader<T, Handler>,
                                        this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred,
                                        boost::ref(t),
                                        boost::make_tuple(handler)));
  }

  // Can throw silvver::connection_error
  template <typename T>
  void write(const T& t);

private:
  static void runIoService();

  /// Length of TCP message header.
  static const unsigned HEADER_LENGTH = 8;

  static boost::asio::io_service ioService;

  static boost::scoped_ptr<boost::thread> th;

  static boost::once_flag onceFlag;

private:
  void beginConstruction(const std::string& serverName,
                         const std::string& receptionistPort);

  template <class T, class Handler>
  void readHeader(const boost::system::error_code& e,
                  std::size_t bytes_transferred,
                  T& t,
                  boost::tuple<Handler> handler);

  template <typename T, typename Handler>
  void readData(const boost::system::error_code& e,
                std::size_t bytes_transferred,
                T& t,
                boost::tuple<Handler> handler);

  std::size_t getDataSize();

  template <class T>
  void extractData(T& t);

  boost::asio::ip::tcp::socket socket;

  char inboundHeader[HEADER_LENGTH];

  std::vector<char> inboundData;
};

#endif // CONNECTION_HPP
