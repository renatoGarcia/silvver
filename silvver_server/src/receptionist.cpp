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

#include <boost/bind.hpp>

#include "log.hpp"
#include "inputFactory.hpp"
#include "ioConnection.ipp"
#include "streamConnection.ipp"

namespace ba = boost::asio;
namespace bip = boost::asio::ip;

Receptionist::Receptionist(unsigned localPort)
  :ioService()
  ,acceptor(this->ioService,
            bip::tcp::endpoint(bip::tcp::v4(), localPort))
  ,currentReception()
  ,request()
  ,mapInputs()
  ,targetOutputs(OutputMultiMap<CLIENT_TARGET, silvver::TargetUid>::instantiate())
  ,cameraOutputs(OutputMultiMap<CLIENT_CAMERA, silvver::AbstractCameraUid>::instantiate())
  ,thReceptionist(new boost::thread(&Receptionist::run, this))
{
  StreamConnection::pointer connection =
    StreamConnection::create(Receptionist::ioService);
  this->acceptor.async_accept(connection->getSocket(),
                              boost::bind(&Receptionist::handleAccept,
                                          this,
                                          connection));
}

Receptionist::~Receptionist()
{
  this->ioService.stop();
  this->thReceptionist->join();
}

void
Receptionist::run()
{
  this->ioService.run();
}

void
Receptionist::handleAccept(StreamConnection::pointer connection)
{
  this->currentReception = connection;

  // Ask the incoming client for request
  connection->read(this->request);
  boost::apply_visitor(*this, this->request);

  // Create a new connection and wait a new client
  StreamConnection::pointer newConnection =
    StreamConnection::create(Receptionist::ioService);

  this->acceptor.async_accept(newConnection->getSocket(),
                              boost::bind(&Receptionist::handleAccept,
                                          this,
                                          newConnection));
}

void
Receptionist::operator()(NullRequest& request)
{
  message(MessageLogLevel::ERROR)
    << ts_output::lock
    << "Null request received" << std::endl
    << ts_output::unlock;
}

void
Receptionist::operator()(AddTargetClient& request)
{
  boost::shared_ptr<IoConnection>
    ioConnection(new IoConnection(this->currentReception->getRemoteIp(),
                                  request.localPort));

  this->currentReception->write(ioConnection->getLocalPort());

  message(MessageLogLevel::INFO)
    << ts_output::lock
    << "Add target client request. Uid: " << request.targetUid << std::endl
    << ts_output::unlock;

  this->targetOutputs->addOutput(request.targetUid, ioConnection);
}

void
Receptionist::operator()(DelTargetClient& request)
{
  message(MessageLogLevel::INFO)
    << ts_output::lock
    << "Delete target client request. Uid: " << request.targetUid << std::endl
    << ts_output::unlock;

  this->targetOutputs->delOutput(request.targetUid, request.localPort);
}

void
Receptionist::operator()(AddCameraClient& request)
{
  boost::shared_ptr<IoConnection>
    ioConnection(new IoConnection(this->currentReception->getRemoteIp(),
                                  request.localPort));

  this->currentReception->write(ioConnection->getLocalPort());

  message(MessageLogLevel::INFO)
    << ts_output::lock
    << "Add camera client request. Uid: " << request.cameraUid << std::endl
    << ts_output::unlock;

  this->cameraOutputs->addOutput(request.cameraUid, ioConnection);
}

void
Receptionist::operator()(DelCameraClient& request)
{
  message(MessageLogLevel::INFO)
    << ts_output::lock
    << "Delete camera client request. Uid: " << request.cameraUid << std::endl
    << ts_output::unlock;

  this->cameraOutputs->delOutput(request.cameraUid, request.localPort);
}

void
Receptionist::operator()(AddCamera& request)
{
  message(MessageLogLevel::INFO)
    << ts_output::lock
    << "Add camera request. Uid: " << request.cameraUid << std::endl
    << ts_output::unlock;

  this->currentReception->setCloseHandler(boost::bind(&Receptionist::closeCamera,
                                                      this,
                                                      request.cameraUid));

  boost::shared_ptr<InputInterface> input =
    InputFactory::createInput(request.processorOpt, this->currentReception);

  this->mapInputs.insert(std::make_pair(request.cameraUid, input));
}

void
Receptionist::closeCamera(const silvver::AbstractCameraUid& cameraUid)
{
  message(MessageLogLevel::INFO)
    << ts_output::lock
    << "Camera Uid: " << cameraUid << " closed connection." << std::endl
    << ts_output::unlock;

  if (this->mapInputs.erase(cameraUid) != 1)
  {
    message(MessageLogLevel::ERROR)
      << ts_output::lock
      << "Trying close unknown camera." << std::endl
      << ts_output::unlock;
  }
}
