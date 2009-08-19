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

#ifndef _IO_CONNECTION_IPP_
#define _IO_CONNECTION_IPP_

#include "ioConnection.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/system/system_error.hpp>
#include <boost/serialization/vector.hpp>
#include "serializations.hpp"

template <typename T>
void
IoConnection::send(const T& t)
{
  std::string outboundData;

  std::ostringstream archiveStream;
  boost::archive::text_oarchive archive(archiveStream);
  archive << t;
  outboundData = archiveStream.str();

  this->socket.send(boost::asio::buffer(outboundData));
}

template <typename T, typename Handler>
void
IoConnection::readData(const boost::system::error_code& e,
                       std::size_t bytes_transferred,
                       T& t,
                       boost::tuple<Handler> handler)
{
  if (e)
  {
    throw boost::system::system_error(e);
  }

  try
  {
    std::string archiveData(&this->inboundData[0], bytes_transferred);
    std::istringstream archiveStream(archiveData);
    boost::archive::text_iarchive archive(archiveStream);
    archive >> t;
  }
  catch (std::exception& e)
  {
    // Unable to decode data.
    throw boost::system::system_error(boost::asio::error::invalid_argument);
  }

  boost::get<0>(handler)();
}

#endif /* _IO_CONNECTION_IPP_ */
