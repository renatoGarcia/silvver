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

#include "receptionist.hpp"

#include <iostream>

#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include "debugOutputs.hpp"
#include "inputFactory.hpp"
#include "ioConnection.ipp"
#include "streamConnection.ipp"

namespace ba = boost::asio;
namespace bip = boost::asio::ip;

boost::asio::io_service Receptionist::ioService;

Receptionist::Receptionist(unsigned localPort)
  :acceptor(Receptionist::ioService,
            bip::tcp::endpoint(bip::tcp::v4(), localPort))
{
  this->outputs = ClientsMap::instantiate();
}

Receptionist::~Receptionist()
{
  this->ioService.stop();
  this->thReceptionist->join();
}

void
Receptionist::run()
{
  if(!this->thReceptionist) // Do anything only if not running yet
  {
    StreamConnection::pointer connection =
      StreamConnection::create(Receptionist::ioService);
    this->acceptor.async_accept(connection->getSocket(),
                                boost::bind(&Receptionist::handleAccept,
                                            this,
                                            connection));

    thReceptionist.reset(new boost::thread(boost::ref(*this)));
  }
}

void
Receptionist::operator()()
{
  Receptionist::ioService.run();
}

void
Receptionist::handleAccept(StreamConnection::pointer connection)
{
  this->currentReception = connection;

  // Ask the incoming client for request
  connection->asyncRead(this->request,
                        boost::bind(&Receptionist::handleRead,
                                    this));

  // Create a new connection and wait a new client
  StreamConnection::pointer newConnection =
    StreamConnection::create(Receptionist::ioService);
  this->acceptor.async_accept(newConnection->getSocket(),
                              boost::bind(&Receptionist::handleAccept,
                                          this,
                                          newConnection));
}

void
Receptionist::handleRead()
{
  boost::apply_visitor(*this, this->request);
}

void
Receptionist::operator()(NullRequest& request) const
{}

void
Receptionist::operator()(AddOutput& request) const
{
  debug::messageOutput(1) << ts_output::lock
                          << "Add output request. Id:" << request.targetId
                          << std::endl
                          << ts_output::unlock;

  boost::shared_ptr<IoConnection>
    ioConnection(new IoConnection(this->currentReception->getRemoteIp(),
                                  request.localPort));

  this->currentReception->write(ioConnection->getLocalPort());
  this->outputs->addOutput(request, ioConnection);
}

void
Receptionist::operator()(DelOutput& request) const
{
  debug::messageOutput(1) << ts_output::lock
                          << "Delete output request" << std::endl
                          << ts_output::unlock;

  this->outputs->removeOutput(request.targetId, request.localPort);
}

void
Receptionist::operator()(AddCamera& request)
{
  debug::messageOutput(1) << ts_output::lock
                          << "Add camera request" << std::endl
                          << ts_output::unlock;

  boost::shared_ptr<IoConnection>
    ioConnection(new IoConnection(this->currentReception->getRemoteIp(),
                                  request.localPort));

  this->currentReception->write(ioConnection->getLocalPort());

  boost::shared_ptr<InputInterface> input =
    InputFactory::createInput(request.targetType, ioConnection);

  this->mapInputs.insert(std::pair< unsigned,boost::shared_ptr<InputInterface> >
                         ((unsigned)ioConnection->getRemotePort(), input));
}

void
Receptionist::operator()(DelCamera& request)
{
  debug::messageOutput(1) << ts_output::lock
                          << "Delete camera request" << std::endl
                          << ts_output::unlock;

  if (this->mapInputs.erase(request.localPort) != 1)
  {
    std::cerr << "Unknown camera delete request" << std::endl;
  }
}
