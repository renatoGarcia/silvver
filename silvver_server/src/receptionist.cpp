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

#include "receptionist.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp>

#include "common/connection/acceptor.ipp"
#include "common/connection/channel.ipp"
#include "common/serializations.hpp"
#include "inputFactory.hpp"
#include "log.hpp"

namespace bip = boost::asio::ip;

Receptionist::Receptionist(unsigned localPort)
  :ioService()
  ,acceptor(this->ioService,
            bip::tcp::endpoint(bip::tcp::v4(), localPort))
  ,currentReception()
  ,mapInputs()
  ,targetOutputs(OutputMultiMap<silvver::TargetUid>::instantiate())
  ,cameraOutputs(OutputMultiMap<silvver::AbstractCameraUid>::instantiate())
  ,thReceptionist(static_cast<std::size_t (boost::asio::io_service::*)()>(&boost::asio::io_service::run),
                  &this->ioService)
{
  this->acceptor.asyncAccept(this->ioService,
                             boost::bind(&Receptionist::handleAccept,
                                         this, _1, _2));
}

Receptionist::~Receptionist()
{
  this->ioService.stop();
  this->thReceptionist.join();
}

void
Receptionist::handleAccept(connection::Channel* channel,
                           connection::error_code ec)
{
  this->currentReception.reset(channel);

  // Ask the incoming client for request
  Request request;
  channel->receive(request);
  boost::apply_visitor(*this, request);

  this->acceptor.asyncAccept(this->ioService,
                             boost::bind(&Receptionist::handleAccept,
                                         this, _1, _2));
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
  message(MessageLogLevel::INFO)
    << ts_output::lock
    << "Add target client request. Uid: " << request.targetUid << std::endl
    << ts_output::unlock;

  this->currentReception->setCloseHandler(boost::bind(&Receptionist::closeTargetClient,
                                                      this,
                                                      request.targetUid,
                                                      this->currentReception));

  this->targetOutputs->addOutput(request.targetUid, this->currentReception);
}

void
Receptionist::closeTargetClient(const silvver::TargetUid& targetUid,
                                boost::shared_ptr<connection::Channel> channel)
{
  message(MessageLogLevel::INFO)
    << ts_output::lock
    << "Closed connection with target client uid: " << targetUid << std::endl
    << ts_output::unlock;

  this->targetOutputs->delOutput(targetUid, channel);
}

void
Receptionist::operator()(AddCameraClient& request)
{
  message(MessageLogLevel::INFO)
    << ts_output::lock
    << "Add camera client request. Uid: " << request.cameraUid << std::endl
    << ts_output::unlock;

  this->currentReception->setCloseHandler(boost::bind(&Receptionist::closeCameraClient,
                                                      this,
                                                      request.cameraUid,
                                                      this->currentReception));

  this->cameraOutputs->addOutput(request.cameraUid, this->currentReception);
}

void
Receptionist::closeCameraClient(const silvver::AbstractCameraUid& cameraUid,
                                boost::shared_ptr<connection::Channel> channel)
{
  message(MessageLogLevel::INFO)
    << ts_output::lock
    << "Closed connection with camera client uid: " << cameraUid << std::endl
    << ts_output::unlock;

  this->cameraOutputs->delOutput(cameraUid, channel);
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
