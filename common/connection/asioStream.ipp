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

#ifndef _ASIO_STREAM_IPP_
#define _ASIO_STREAM_IPP_

#include "asioStream.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <iomanip>
#include <sstream>

#include "channel.ipp"
#include "serializator.hpp"

namespace connection {

namespace bip = boost::asio::ip;

template <class Type>
AsioStream<Type>::AsioStream(boost::asio::io_service& ioService)
  :Channel(Type::portableClassId)
  ,socket(ioService)
  ,inboundHeader()
  ,inboundData()
{}

template <class Type>
AsioStream<Type>::AsioStream(boost::asio::io_service& ioService,
                             const Endpoint& localEp)
  :Channel(Type::portableClassId)
  ,socket(ioService, localEp)
  ,inboundHeader()
  ,inboundData()
{}

template <class Type>
AsioStream<Type>::~AsioStream() throw()
{
  try
  {
    this->close();
  }
  catch(...)
  {}
}

template <class Type>
void
AsioStream<Type>::write(const std::string& data)
{
  std::string header;
  std::ostringstream headerStream;
  headerStream << std::setw(AsioStream<Type>::HEADER_LENGTH)
               << std::hex << data.size();
  if (!headerStream ||
      (headerStream.str().size() != AsioStream<Type>::HEADER_LENGTH))
  {
    throw data_error("Error creating package header.");
  }
  header = headerStream.str();

  std::vector<boost::asio::const_buffer> buffers;
  buffers.push_back(boost::asio::buffer(header));
  buffers.push_back(boost::asio::buffer(data));

  boost::system::error_code ec;
  boost::asio::write(this->socket, buffers,
                     boost::asio::transfer_all(), ec);
  if (ec)
  {
    if (ec == boost::asio::error::connection_aborted ||
        ec == boost::asio::error::broken_pipe)
      throw broken_connection("Lost connection");
    else
      throw data_error(ec.message());
  }
}

template <class Type>
void
AsioStream<Type>::read(std::string& data)
{
  boost::system::error_code ec;
  char inHeader[AsioStream<Type>::HEADER_LENGTH];
  boost::asio::read(this->socket, boost::asio::buffer(inHeader),
                    boost::asio::transfer_all(), ec);
  if (ec)
  {
    if (ec == boost::asio::error::connection_aborted ||
        ec == boost::asio::error::broken_pipe)

      throw broken_connection("Lost connection");
    else
      throw data_error(ec.message());
  }

  // Determine the length of the serialized data.
  std::istringstream is(std::string(inHeader,
                                    AsioStream<Type>::HEADER_LENGTH));
  std::size_t inboundDataSize = 0;
  if (!(is >> std::hex >> inboundDataSize))
  {
    throw data_error("Error extracting package header.");
  }

  std::vector<char> inData(inboundDataSize);
  boost::asio::read(this->socket, boost::asio::buffer(inData),
                    boost::asio::transfer_all(), ec);
  if (ec)
  {
    if (ec == boost::asio::error::connection_aborted ||
        ec == boost::asio::error::broken_pipe)
      throw broken_connection("Lost connection");
    else
      throw data_error(ec.message());
  }

  data.assign(inData.begin(), inData.end());
}

template <class Type>
void
AsioStream<Type>::asyncRead(boost::shared_ptr<std::string> data,
                            ReadHandler handler)
{
  boost::asio::async_read(this->socket,
                          boost::asio::buffer(this->inboundHeader),
                          boost::bind(&AsioStream<Type>::readHeader,
                                      this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred,
                                      data,
                                      handler));
}

template <class Type>
void
AsioStream<Type>::close()
{
  if (this->socket.is_open())
  {
    this->socket.shutdown(Socket::shutdown_both);
    this->socket.close();
  }
}

template <class Type>
void
AsioStream<Type>::effectConnection(const std::string& strAcceptorEp)
{
  Endpoint acceptorEp;
  deserialize(acceptorEp, strAcceptorEp);

  this->socket.connect(acceptorEp);
}

template <class Type>
bool
AsioStream<Type>::getDataSize(std::size_t& size) const
{
  // Determine the length of the serialized data.
  std::istringstream is(std::string(this->inboundHeader, HEADER_LENGTH));
  std::size_t inboundDataSize = 0;
  if (!(is >> std::hex >> inboundDataSize))
  {
    size = 0;
    return false;
  }

  size = inboundDataSize;
  return true;
}

template <class Type>
void
AsioStream<Type>::readHeader(const boost::system::error_code& e,
                             std::size_t bytes_transferred,
                             boost::shared_ptr<std::string> data,
                             ReadHandler handler)
{
  if (e)
  {
    handler(e, bytes_transferred);
    return;
  }

  std::size_t size;
  if (!this->getDataSize(size))
  {
    handler(boost::asio::error::message_size, bytes_transferred);
    return;
  }
  this->inboundData.resize(size);

  boost::asio::async_read
    (this->socket,
     boost::asio::buffer(this->inboundData),
     boost::bind(&AsioStream<Type>::readData,
                 this,
                 boost::asio::placeholders::error,
                 boost::asio::placeholders::bytes_transferred,
                 data,
                 handler));
}

template <class Type>
void
AsioStream<Type>::readData(const boost::system::error_code& e,
                           std::size_t bytes_transferred,
                           boost::shared_ptr<std::string> data,
                           ReadHandler handler) const
{
  if (e)
  {
    handler(e, bytes_transferred);
    return;
  }

  data->assign(this->inboundData.begin(), this->inboundData.end());

  // Inform caller that data has been received ok.
  handler(e, bytes_transferred);
}

} // namespace connection

#endif /* _ASIO_STREAM_IPP_ */
