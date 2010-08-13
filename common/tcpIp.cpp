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

#include "tcpIp.hpp"

#include <boost/bind.hpp>
#include <boost/serialization/vector.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace bip = boost::asio::ip;

boost::asio::io_service TcpIp::ioService;
boost::scoped_ptr<boost::thread> TcpIp::th;
boost::once_flag TcpIp::onceFlag = BOOST_ONCE_INIT;

void
TcpIp::runIoService()
{
  TcpIp::ioService.run();
}

std::vector<TcpIpEp>
TcpIp::resolve(const std::string& address, const std::string& port)
{
  boost::call_once(TcpIp::onceFlag,
                   boost::bind(&boost::scoped_ptr<boost::thread>::reset,
                               &TcpIp::th,
                               new boost::thread(TcpIp::runIoService)));

  std::vector<TcpIpEp> endpoints;
  bip::tcp::resolver resolver(TcpIp::ioService);
  bip::tcp::resolver::query query(address, port);

  bip::tcp::resolver::iterator endpoint_iterator, end;
  for (endpoint_iterator = resolver.resolve(query);
       endpoint_iterator != end;
       ++endpoint_iterator)
  {
    endpoints.push_back(TcpIpEp(*endpoint_iterator));
  }

  return endpoints;
}

TcpIp::TcpIp(const TcpIpEp& localEp)
  :Channel()
  ,localEndpoint(localEp)
  ,socket(TcpIp::ioService)
{
  boost::call_once(TcpIp::onceFlag,
                   boost::bind(&boost::scoped_ptr<boost::thread>::reset,
                               &TcpIp::th,
                               new boost::thread(TcpIp::runIoService)));
}

TcpIp::TcpIp(const TcpIpEp& localEp, const TcpIpEp& remoteEp)
  :Channel()
  ,localEndpoint(localEp)
  ,socket(TcpIp::ioService, localEp.ep)
{
  boost::call_once(TcpIp::onceFlag,
                   boost::bind(&boost::scoped_ptr<boost::thread>::reset,
                               &TcpIp::th,
                               new boost::thread(TcpIp::runIoService)));

  this->socket.connect(remoteEp.ep);
}

TcpIp::~TcpIp() throw()
{
  try
  {
    this->close();
  }
  catch(...)
  {}
}

void
TcpIp::close()
{
  if(this->socket.is_open())
  {
    this->socket.shutdown(bip::tcp::socket::shutdown_both);
    this->socket.close();
  }
}

void
TcpIp::accept()
{
  bip::tcp::acceptor acceptor(TcpIp::ioService, this->localEndpoint.ep);
  acceptor.accept(this->socket);
}

Endpoint
TcpIp::getEndpoint() const
{
  return this->localEndpoint;
}

void
TcpIp::send(const std::string& data)
{
  std::string header;
  std::ostringstream headerStream;
  headerStream << std::setw(TcpIp::HEADER_LENGTH)
               << std::hex << data.size();
  if (!headerStream || (headerStream.str().size()!=TcpIp::HEADER_LENGTH))
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

void
TcpIp::receive(std::string& data)
{
  boost::system::error_code ec;
  char inHeader[TcpIp::HEADER_LENGTH];
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
  std::istringstream is(std::string(inHeader, TcpIp::HEADER_LENGTH));
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
