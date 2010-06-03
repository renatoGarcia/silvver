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

#include "connection.hpp"
#include "connection.ipp"

#include <boost/bind.hpp>
#include <iostream>

#include "exceptions.hpp"

namespace bip = boost::asio::ip;

boost::asio::io_service Connection::ioService;
boost::scoped_ptr<boost::thread> Connection::th;
boost::once_flag Connection::onceFlag = BOOST_ONCE_INIT;

void
Connection::runIoService()
{
  Connection::ioService.run();
}

Connection::Connection(const std::string& serverName,
                       const std::string& receptionistPort,
                       const Request& request)
  :socket(Connection::ioService, bip::tcp::endpoint())
{
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
    socket.close();
    socket.connect(*endpoint_iterator, error);
    ++endpoint_iterator;
  }
  if (error)
  {
    throw server_connection_error()
      << info_what("Server " + serverName + " not found.");
  }

  this->write(request);
}

Connection::~Connection()
{
  if(this->socket.is_open())
  {
    try
    {
      this->socket.shutdown(bip::tcp::socket::shutdown_both);
      this->socket.close();
    }
    catch(...)
    {
      std::cerr << "Error on closing the connection with server" << std::endl;
    }
  }
}
