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

#ifndef CONNECTION_IPP
#define CONNECTION_IPP

#include "connection.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <sstream>

#include "exceptions.hpp"
#include "serializations.hpp"

namespace bip = boost::asio::ip;

template <typename T>
void
Connection::write(const T& t)
{
  std::string outboundData;
  std::string outboundHeader;

  // Serialize the data first so we know how large it is.
  std::ostringstream archiveStream;
  boost::archive::text_oarchive archive(archiveStream);
  archive << t;
  outboundData = archiveStream.str();

  // Format the header.
  std::ostringstream headerStream;
  headerStream << std::setw(Connection::HEADER_LENGTH)
               << std::hex << outboundData.size();
  if (!headerStream || headerStream.str().size() != Connection::HEADER_LENGTH)
  {
//     // Something went wrong, inform the caller.
//     boost::system::error_code error(boost::asio::error::invalid_argument);
//     socket_.io_service().post(boost::bind(handler, error));
    return;
  }
  outboundHeader = headerStream.str();

  std::vector<boost::asio::const_buffer> buffers;
  buffers.push_back(boost::asio::buffer(outboundHeader));
  buffers.push_back(boost::asio::buffer(outboundData));

  try
  {
    boost::asio::write(this->socket, buffers);
  }
  catch (const boost::system::system_error& e)
  {
    throw silvver::connection_error("Error writing to silvver-server");
  }
}

template <class T>
void
Connection::extractData(T& t)
{
  // Extract the data structure from the data just received.
  try
  {
    std::string archiveData(&this->inboundData[0], this->inboundData.size());
    std::istringstream archiveStream(archiveData);
    boost::archive::text_iarchive archive(archiveStream);
    archive >> t;
  }
  catch (const std::exception& e)
  {
    throw silvver::connection_error("Unable to decode data read from silvver-server");
  }
}

template <typename T>
void
Connection::read(T& t)
{
  boost::asio::read(this->socket, boost::asio::buffer(this->inboundHeader));
  this->inboundData.resize(this->getDataSize());

  boost::asio::read(this->socket, boost::asio::buffer(this->inboundData));
  this->extractData(t);
}

// Handle a completed read of a message header. The handler is passed using
// a tuple since boost::bind seems to have trouble binding a function object
// created using boost::bind as a parameter.
template <typename T, typename Handler>
void
Connection::readHeader(const boost::system::error_code& e,
                       std::size_t bytes_transferred,
                       T& t,
                       boost::tuple<Handler> handler)
{
  if (e)
  {
    // this->close();
    return;
  }

  try
  {
    this->inboundData.resize(this->getDataSize());
  }
  catch (const silvver::connection_error& e)
  {
    // this->close();
    return;
  }

  boost::asio::async_read
    (this->socket,
     boost::asio::buffer(this->inboundData),
     boost::bind(&Connection::readData<T, Handler>,
                 this,
                 boost::asio::placeholders::error,
                 boost::asio::placeholders::bytes_transferred,
                 boost::ref(t),
                 handler));
}

template <typename T, typename Handler>
void
Connection::readData(const boost::system::error_code& e,
                     std::size_t bytes_transferred,
                     T& t,
                     boost::tuple<Handler> handler)
{
  if (e)
  {
    // this->close();
    return;
  }

  try
  {
    this->extractData(t);
  }
  catch (const silvver::connection_error& e)
  {
    // this->close();
    return;
  }

  boost::get<0>(handler)();
}

#endif // CONNECTION_IPP
