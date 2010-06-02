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

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/serialization/variant.hpp>
#include <iostream>
#include <sstream>
#include <vector>

#include "request.hpp"

namespace bip = boost::asio::ip;

boost::asio::io_service Connection::ioService;
boost::scoped_ptr<boost::thread> Connection::th;
boost::once_flag Connection::onceFlag = BOOST_ONCE_INIT;

void
Connection::runIoService()
{
  Connection::ioService.run();
}

Connection::Connection(const std::string& serverIp, unsigned receptionistPort,
                       const silvver::AbstractCameraUid& abstractCameraUid)
  :abstractCameraUid(abstractCameraUid)
  ,receptionistEP(bip::address::from_string(serverIp), receptionistPort)
  ,socket(Connection::ioService, bip::tcp::endpoint())
{
  boost::call_once(Connection::onceFlag,
                   boost::bind(&boost::scoped_ptr<boost::thread>::reset,
                               &Connection::th,
                               new boost::thread(Connection::runIoService)));
}

Connection::~Connection()
{
  if(this->socket.is_open())
  {
    try
    {
      this->disconnect();
    }
    catch(...)
    {
      std::cerr << "Error on closing the connection with server" << std::endl;
    }
  }
}

void
Connection::connect(const procOpt::AnyProcOpt& processorOpt)
{
  // bip::tcp::socket receptionistSocket(Connection::ioService);
  // receptionistSocket.connect(this->receptionistEP);
  this->socket.connect(this->receptionistEP);

  Request request = AddCamera(processorOpt, this->abstractCameraUid);
  this->write(request, this->socket);

  // unsigned short remotePort;
  // this->read(remotePort, receptionistSocket);

  // this->socket.connect(bip::tcp::endpoint(this->receptionistEP.address(),
  //                                         remotePort));

  // receptionistSocket.shutdown(bip::tcp::socket::shutdown_both);
  // receptionistSocket.close();
}

void
Connection::disconnect()
{
  bip::tcp::socket receptionistSocket(Connection::ioService);
  receptionistSocket.connect(this->receptionistEP);

  Request request = DelCamera(this->abstractCameraUid);
  this->write(request, receptionistSocket);

  this->socket.shutdown(bip::tcp::socket::shutdown_both);
  this->socket.close();

  receptionistSocket.shutdown(bip::tcp::socket::shutdown_both);
  receptionistSocket.close();
}
