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

#ifndef _STREAM_CONNECTION_HPP_
#define _STREAM_CONNECTION_HPP_

#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

class StreamConnection
  :public boost::enable_shared_from_this<StreamConnection>
{
public:
  typedef boost::shared_ptr<StreamConnection> pointer;

  inline static pointer
  create(boost::asio::io_service& ioService)
  {
    return pointer(new StreamConnection(ioService));
  }

  inline boost::asio::ip::tcp::socket&
  getSocket()
  {
    return this->socket;
  }

  inline std::string
  getRemoteIp()
  {
    return this->socket.remote_endpoint().address().to_string();
  }

  inline void
  setCloseHandler(const boost::function<void (void)>& closeHandler)
  {
    this->closeHandler = closeHandler;
  }

  /** Read a value of type T.
   * @param t Reference to the variable where the read value will be returned. */
  template <class T>
  void read(T& t);

  /** Asynchronously read a value of type T.
   * @param t Reference to the variable where the read value will be returned.
   * @param handler Function with signature "void handler()" which to be called
   *                when the operation completes.  */
  template <class T, class Handler>
  inline void
  asyncRead(T& t, Handler handler)
  {
    boost::asio::async_read(this->socket,
                            boost::asio::buffer(inboundHeader),
                            boost::bind(&StreamConnection::readHeader<T, Handler>,
                                        shared_from_this(),
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred,
                                        boost::ref(t),
                                        boost::make_tuple(handler)));
  }

  template <class T>
  void write(const T& t);

private:
  StreamConnection(boost::asio::io_service& ioService)
    :socket(ioService)
  {}

  /// Analise inboundHeader attribute to get the next incoming data size.
  std::size_t getDataSize();

  /** Extract the last received data package.
   * @param t Variable where put the extracted data. */
  template <class T>
  void extractData(T& t);

  template <class T, class Handler>
  void readHeader(const boost::system::error_code& e,
                  std::size_t bytes_transferred,
                  T& t,
                  boost::tuple<Handler> handler);

  template <class T, class Handler>
  void readData(const boost::system::error_code& e,
                std::size_t bytes_transferred,
                T& t,
                boost::tuple<Handler> handler);

  static const unsigned HEADER_LENGTH = 8;

  boost::asio::ip::tcp::socket socket;

  char inboundHeader[HEADER_LENGTH];

  std::vector<char> inboundData;

  boost::function<void (void)> closeHandler;
};

#endif /* _STREAM_CONNECTION_HPP_ */
