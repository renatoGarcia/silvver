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

#include "connection.hpp"
#include "connection.ipp"

#include "exceptions.hpp"
#include "request.hpp"

namespace bip = boost::asio::ip;

boost::asio::io_service Connection::ioService;
boost::scoped_ptr<boost::thread> Connection::th;
boost::once_flag Connection::onceFlag = BOOST_ONCE_INIT;

void
Connection::runIoService()
{
  // boost::asio::io_service::work work(Connection::ioService);

  // while (true)
  // {
    try
    {
      Connection::ioService.run();
    }
    // This system_error is threw when closing the udp socket, and there are
    // any asynchronous send, receive or connect operations yet.
    catch(boost::system::system_error& e)
    {}
  // }
}

Connection::Connection(const std::string& serverName,
                       const std::string& receptionistPort,
                       const silvver::TargetUid& targetUid)
  :socket(Connection::ioService)
{
  this->beginConstruction(serverName, receptionistPort);

  Request request = AddTargetClient(targetUid);
  this->write(request);
}

Connection::Connection(const std::string& serverName,
                       const std::string& receptionistPort,
                       const silvver::AbstractCameraUid& cameraUid)

  :socket(Connection::ioService)
{
  this->beginConstruction(serverName, receptionistPort);

  Request request = AddCameraClient(cameraUid);
  this->write(request);
}

void
Connection::close()
{
  if(this->socket.is_open())
  {
    this->socket.shutdown(bip::tcp::socket::shutdown_both);
    this->socket.close();
  }
}

Connection::~Connection() throw()
{
  try
  {
    this->close();
  }
  catch(...)
  {}
}

void
Connection::beginConstruction(const std::string& serverName,
                              const std::string& receptionistPort)
{
  // Only one static io_service to all connection classes
  boost::call_once(Connection::onceFlag,
                   boost::bind(&boost::scoped_ptr<boost::thread>::reset,
                               &Connection::th,
                               new boost::thread(Connection::runIoService)));

  bip::tcp::resolver resolver(Connection::ioService);
  bip::tcp::resolver::query query(serverName, receptionistPort);

  bip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
  bip::tcp::resolver::iterator end;
  boost::system::error_code error = boost::asio::error::host_not_found;
  while (error && endpoint_iterator != end)
  {
    this->socket.close();
    this->socket.connect(*endpoint_iterator, error);
    ++endpoint_iterator;
  }
  if (error)
  {
    throw silvver::connection_error("Server " + serverName + " not found.");
  }
}

std::size_t
Connection::getDataSize()
{
  // Determine the length of the serialized data.
  std::istringstream is(std::string(this->inboundHeader,
                                    Connection::HEADER_LENGTH));
  std::size_t inboundDataSize = 0;
  if (!(is >> std::hex >> inboundDataSize))
  {
    // throw boost::system::system_error(boost::asio::error::invalid_argument);
    throw silvver::connection_error("Error on reading from silvver-server");
  }

  return inboundDataSize;
}
