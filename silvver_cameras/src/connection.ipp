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

#ifndef _CONNECTION_IPP_
#define _CONNECTION_IPP_

#include "connection.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <sstream>
#include <vector>

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
  if (!headerStream || (headerStream.str().size()!=Connection::HEADER_LENGTH))
  {
    throw boost::system::system_error(boost::asio::error::invalid_argument);
  }
  outboundHeader = headerStream.str();

  std::vector<boost::asio::const_buffer> buffers;
  buffers.push_back(boost::asio::buffer(outboundHeader));
  buffers.push_back(boost::asio::buffer(outboundData));

  boost::asio::write(this->socket, buffers);
}

template <typename T>
void
Connection::read(T& t)
{
  char inHeader[Connection::HEADER_LENGTH];
  boost::asio::read(this->socket, boost::asio::buffer(inHeader));

  // Determine the length of the serialized data.
  std::istringstream is(std::string(inHeader, Connection::HEADER_LENGTH));
  std::size_t inboundDataSize = 0;
  if (!(is >> std::hex >> inboundDataSize))
  {
    throw boost::system::system_error(boost::asio::error::invalid_argument);
  }

  std::vector<char> inData(inboundDataSize);
  boost::asio::read(this->socket, boost::asio::buffer(inData));

  // Extract the data structure from the data just received.
  try
  {
    std::string archiveData(&inData[0], inData.size());
    std::istringstream archiveStream(archiveData);
    boost::archive::text_iarchive archive(archiveStream);
    archive >> t;
  }
  catch (std::exception& e)
  {
    // Unable to decode data.
    throw boost::system::system_error(boost::asio::error::invalid_argument);
  }
}

#endif // _CONNECTION_IPP_
