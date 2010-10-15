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

#include "abstractCamera.hpp"

#include "../globalOptions.hpp"
#include "../hardCameraFactory.hpp"
#include "common/connection/channelTypes.hpp"
#include "common/connection/tcpIp.hpp"
#include "common/connection/unixSocket.hpp"
#include "common/request.hpp"

extern globalOptions::Options global_options;

boost::asio::io_service AbstractCamera::ioService;

AbstractCamera::AbstractCamera(const scene::Camera& cameraConfig,
                               const unsigned silvverUid,
                               const procOpt::AnyProcOpt& processorOptions)
  :subjectHardCamera(HardCameraFactory::create(cameraConfig.hardware))
  ,currentFrame()
  ,abstractCameraUid(silvverUid, subjectHardCamera->silvverUid)
  ,serverChannel(AbstractCamera::createChannel(global_options.receptionistEp))
  ,unreadImage(false)
  ,unreadImageAccess()
  ,unreadImageCondition()
  ,rot(cameraConfig.rotationMatrix)
  ,trans(cameraConfig.translationVector)
{
  this->serverChannel->connect<connection::TcpIp>(global_options.receptionistEp);
  Request request = AddCamera(processorOptions, this->abstractCameraUid);
  this->serverChannel->send(request);
  this->subjectHardCamera->attach(this);
}

AbstractCamera::~AbstractCamera()
{
  this->subjectHardCamera->detach(this);
}

void
AbstractCamera::update()
{
  this->unreadImageAccess.lock();
  this->unreadImage = true;
  this->unreadImageAccess.unlock();

  this->unreadImageCondition.notify_one();
}

void
AbstractCamera::updateFrame()
{
  boost::unique_lock<boost::mutex> lock(this->unreadImageAccess);
  while (!this->unreadImage)
  {
    this->unreadImageCondition.wait(lock);
  }

  this->subjectHardCamera->getUnwarpedFrame(this->currentFrame);

  this->unreadImage = false;
}

void
AbstractCamera::toWorld(silvver::Pose &pose) const
{
  silvver::Pose tempPose(pose);

  //------------------------------- Rmo = Rco x Rmc
  pose.rotationMatrix[0] =
    this->rot[0] * tempPose.rotationMatrix[0] +
    this->rot[1] * tempPose.rotationMatrix[3] +
    this->rot[2] * tempPose.rotationMatrix[6];
  pose.rotationMatrix[1] =
    this->rot[0] * tempPose.rotationMatrix[1] +
    this->rot[1] * tempPose.rotationMatrix[4] +
    this->rot[2] * tempPose.rotationMatrix[7];
  pose.rotationMatrix[2] =
    this->rot[0] * tempPose.rotationMatrix[2] +
    this->rot[1] * tempPose.rotationMatrix[5] +
    this->rot[2] * tempPose.rotationMatrix[8];
  pose.rotationMatrix[3] =
    this->rot[3] * tempPose.rotationMatrix[0] +
    this->rot[4] * tempPose.rotationMatrix[3] +
    this->rot[5] * tempPose.rotationMatrix[6];
  pose.rotationMatrix[4] =
    this->rot[3] * tempPose.rotationMatrix[1] +
    this->rot[4] * tempPose.rotationMatrix[4] +
    this->rot[5] * tempPose.rotationMatrix[7];
  pose.rotationMatrix[5] =
    this->rot[3] * tempPose.rotationMatrix[2] +
    this->rot[4] * tempPose.rotationMatrix[5] +
    this->rot[5] * tempPose.rotationMatrix[8];
  pose.rotationMatrix[6] =
    this->rot[6] * tempPose.rotationMatrix[0] +
    this->rot[7] * tempPose.rotationMatrix[3] +
    this->rot[8] * tempPose.rotationMatrix[6];
  pose.rotationMatrix[7] =
    this->rot[6] * tempPose.rotationMatrix[1] +
    this->rot[7] * tempPose.rotationMatrix[4] +
    this->rot[8] * tempPose.rotationMatrix[7];
  pose.rotationMatrix[8] =
    this->rot[6] * tempPose.rotationMatrix[2] +
    this->rot[7] * tempPose.rotationMatrix[5] +
    this->rot[8] * tempPose.rotationMatrix[8];

  //------------------------------- Tmo = Tco + Rco x Tmc
  pose.x = this->trans[0] +
    this->rot[0] * tempPose.x +
    this->rot[1] * tempPose.y +
    this->rot[2] * tempPose.z;
  pose.y = this->trans[1] +
    this->rot[3] * tempPose.x +
    this->rot[4] * tempPose.y +
    this->rot[5] * tempPose.z;
  pose.z = this->trans[2] +
    this->rot[6] * tempPose.x +
    this->rot[7] * tempPose.y +
    this->rot[8] * tempPose.z;
}

connection::Channel*
AbstractCamera::createChannel(const connection::TcpIpEp& receptionistEp)
{
  // if (receptionistEp.address().to_string() == "127.0.0.1")
  // {
    return new connection::UnixSocket(AbstractCamera::ioService);
  // }
  // else
  // {
  //   return new connection::TcpIp(AbstractCamera::ioService);
  // }
}
