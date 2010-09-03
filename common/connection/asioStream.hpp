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

#ifndef _ASIO_STREAM_HPP_
#define _ASIO_STREAM_HPP_

#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <string>
#include <vector>

#include "channel.hpp"
#include "channelTypes.hpp"

/** Class template which wrap a asio socket class as a Channel.
 * The template argument Type must be a class that fulfill a boost asio socket
 * with stream protocol, like boost::asio::ip::tcp or
 * boost::asio::local::stream_protocol.
 */
template <class Type>
class AsioStream
  :public Channel
{
public:
  typedef typename Type::AsioClass::endpoint Endpoint;
  typedef typename Type::AsioClass::socket   Socket;
  typedef typename Type::AsioClass::acceptor Acceptor;

  AsioStream(boost::asio::io_service& ioService);

  AsioStream(boost::asio::io_service& ioService, const Endpoint& localEp);

  virtual ~AsioStream() throw();

  virtual void write(const std::string& data);

  virtual void read(std::string& data);

  virtual void asyncRead(boost::shared_ptr<std::string> data,
                         ReadHandler handler);

  virtual AnyEndpoint localEndpoint() const
  {
    return this->socket.local_endpoint();
  }

  virtual void close();

  Socket& getSocket()
  {
    return this->socket;
  }

private:
  /// Length of message header.
  static const unsigned HEADER_LENGTH = 8;

  virtual void effectConnection(const std::string& strAcceptorEp);

  bool getDataSize(std::size_t& size) const;

  void readHeader(const boost::system::error_code& e,
                  std::size_t bytes_transferred,
                  boost::shared_ptr<std::string> data,
                  ReadHandler handler);

  void readData(const boost::system::error_code& e,
                std::size_t bytes_transferred,
                boost::shared_ptr<std::string> data,
                ReadHandler handler) const;

  Socket socket;

  // Arrays used in async operations
  char inboundHeader[HEADER_LENGTH];
  std::vector<char> inboundData;

};

#endif /* _ASIO_STREAM_HPP_ */
