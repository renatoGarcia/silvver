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

#include "connection.ipp"

#include <iostream>

#include "request.hpp"

namespace bip = boost::asio::ip;

boost::asio::io_service Connection::ioService;
boost::scoped_ptr<boost::thread> Connection::th;
boost::mutex Connection::runMutex;
bool Connection::ioServiceRunning = false;

void
Connection::runIoService()
{
  boost::asio::io_service::work work(Connection::ioService);

  while (true)
  {
    try
    {
      Connection::ioService.run();
    }
    // This system_error is threw when closing the udp socket, and there are
    // any asynchronous send, receive or connect operations yet.
    catch(boost::system::system_error& e)
    {}
  }
}

Connection::Connection(const std::string& serverIp, unsigned receptionistPort)
  :receptionistSocket(Connection::ioService)
  ,receptionistEP(bip::address::from_string(serverIp), receptionistPort)
  ,inputSocket(Connection::ioService)
  ,inboundData(UPD_MAX_LENGTH)
{}

Connection::~Connection()
{
  if(this->inputSocket.is_open())
  {
    try
    {
      this->inputSocket.shutdown(bip::udp::socket::shutdown_both);
      this->inputSocket.close();
    }
    catch(...)
    {}
  }
}

void
Connection::connect(ClientType clientType, unsigned targetId)
{
  if (!Connection::ioServiceRunning)
  {
    boost::mutex::scoped_lock lock(Connection::runMutex);
    if (!Connection::ioServiceRunning)
    {
      Connection::th.reset(new boost::thread(Connection::runIoService));
      Connection::ioServiceRunning = true;
    }
  }

  this->inputSocket.open(bip::udp::v4());
  this->inputSocket.bind(bip::udp::endpoint());

  this->receptionistSocket.connect(this->receptionistEP);

  Request request = AddOutput(clientType, targetId,
                              this->inputSocket.local_endpoint().port());
  this->writeToReceptionist(request);

  unsigned short remotePort;
  this->readFromReceptionist(remotePort);

  this->inputSocket.connect(bip::udp::endpoint(this->receptionistEP.address(),
                                               remotePort));

  this->receptionistSocket.shutdown(bip::tcp::socket::shutdown_both);
  this->receptionistSocket.close();
}

void
Connection::disconnect(ClientType clientType, unsigned targetId)
{
  this->receptionistSocket.connect(this->receptionistEP);

  Request request = DelOutput(clientType, targetId,
                              this->inputSocket.local_endpoint().port());
  this->writeToReceptionist(request);

  this->inputSocket.shutdown(bip::udp::socket::shutdown_both);
  this->inputSocket.close();

  this->receptionistSocket.shutdown(bip::tcp::socket::shutdown_both);
  this->receptionistSocket.close();
}
