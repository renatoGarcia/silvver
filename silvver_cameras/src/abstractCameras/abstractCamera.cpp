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

AbstractCamera::AbstractCamera(const scene::AnyHardCamera& anyHardCamera,
                               const scene::TargetSet& targetSet,
                               const procOpt::AnyProcOpt& processorOptions)
  :subjectHardCamera(HardCameraFactory::create(anyHardCamera))
  ,currentFrame()
  ,abstractCameraUid(targetSet.targetSetUid, subjectHardCamera->hardCameraUid)
  ,serverChannel(AbstractCamera::createChannel(global_options.receptionistEp))
  ,unreadImage(false)
  ,unreadImageAccess()
  ,unreadImageCondition()
  ,hardCameraRotation(AbstractCamera::getRotationMatrix(anyHardCamera))
  ,hardCameraTranslation(AbstractCamera::getTranslationVector(anyHardCamera))
  ,bodyTranslation(targetSet.bodyTranslation)
  ,bodyRotation(targetSet.bodyRotation)
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
AbstractCamera::localizeBody(silvver::Pose& pose) const
{
  silvver::Pose tempPose(pose);

  //------------------------------- Rbc = Rtc x Rbt
  // Rotation of body in camera coordinates is Rotation of target in camera
  // coordinates vec rotation of body in target coordinates
  pose.rotationMatrix[0] =
    tempPose.rotationMatrix[0] * this->bodyRotation[0] +
    tempPose.rotationMatrix[1] * this->bodyRotation[3] +
    tempPose.rotationMatrix[2] * this->bodyRotation[6];
  pose.rotationMatrix[1] =
    tempPose.rotationMatrix[0] * this->bodyRotation[1] +
    tempPose.rotationMatrix[1] * this->bodyRotation[4] +
    tempPose.rotationMatrix[2] * this->bodyRotation[7];
  pose.rotationMatrix[2] =
    tempPose.rotationMatrix[0] * this->bodyRotation[2] +
    tempPose.rotationMatrix[1] * this->bodyRotation[5] +
    tempPose.rotationMatrix[2] * this->bodyRotation[8];
  pose.rotationMatrix[3] =
    tempPose.rotationMatrix[3] * this->bodyRotation[0] +
    tempPose.rotationMatrix[4] * this->bodyRotation[3] +
    tempPose.rotationMatrix[5] * this->bodyRotation[6];
  pose.rotationMatrix[4] =
    tempPose.rotationMatrix[3] * this->bodyRotation[1] +
    tempPose.rotationMatrix[4] * this->bodyRotation[4] +
    tempPose.rotationMatrix[5] * this->bodyRotation[7];
  pose.rotationMatrix[5] =
    tempPose.rotationMatrix[3] * this->bodyRotation[2] +
    tempPose.rotationMatrix[4] * this->bodyRotation[5] +
    tempPose.rotationMatrix[5] * this->bodyRotation[8];
  pose.rotationMatrix[6] =
    tempPose.rotationMatrix[6] * this->bodyRotation[0] +
    tempPose.rotationMatrix[7] * this->bodyRotation[3] +
    tempPose.rotationMatrix[8] * this->bodyRotation[6];
  pose.rotationMatrix[7] =
    tempPose.rotationMatrix[6] * this->bodyRotation[1] +
    tempPose.rotationMatrix[7] * this->bodyRotation[4] +
    tempPose.rotationMatrix[8] * this->bodyRotation[7];
  pose.rotationMatrix[8] =
    tempPose.rotationMatrix[6] * this->bodyRotation[2] +
    tempPose.rotationMatrix[7] * this->bodyRotation[5] +
    tempPose.rotationMatrix[8] * this->bodyRotation[8];

  //------------------------------- Tbc = Ttc + Rtc x Tbt
  // Translation of body in camera coordinates is translation of target in
  // camera coordinates plus rotation of target in camera coordinates vec
  // translation of body in target coordinates
  pose.x = tempPose.x +
    tempPose.rotationMatrix[0] * this->bodyTranslation[0] +
    tempPose.rotationMatrix[1] * this->bodyTranslation[1] +
    tempPose.rotationMatrix[2] * this->bodyTranslation[2];
  pose.y = tempPose.y +
    tempPose.rotationMatrix[3] * this->bodyTranslation[0] +
    tempPose.rotationMatrix[4] * this->bodyTranslation[1] +
    tempPose.rotationMatrix[5] * this->bodyTranslation[2];
  pose.z = tempPose.z +
    tempPose.rotationMatrix[6] * this->bodyTranslation[0] +
    tempPose.rotationMatrix[7] * this->bodyTranslation[1] +
    tempPose.rotationMatrix[8] * this->bodyTranslation[2];
}

void
AbstractCamera::toWorld(silvver::Pose &pose) const
{
  silvver::Pose tempPose(pose);

  //------------------------------- Rmo = Rco x Rmc
  pose.rotationMatrix[0] =
    this->hardCameraRotation[0] * tempPose.rotationMatrix[0] +
    this->hardCameraRotation[1] * tempPose.rotationMatrix[3] +
    this->hardCameraRotation[2] * tempPose.rotationMatrix[6];
  pose.rotationMatrix[1] =
    this->hardCameraRotation[0] * tempPose.rotationMatrix[1] +
    this->hardCameraRotation[1] * tempPose.rotationMatrix[4] +
    this->hardCameraRotation[2] * tempPose.rotationMatrix[7];
  pose.rotationMatrix[2] =
    this->hardCameraRotation[0] * tempPose.rotationMatrix[2] +
    this->hardCameraRotation[1] * tempPose.rotationMatrix[5] +
    this->hardCameraRotation[2] * tempPose.rotationMatrix[8];
  pose.rotationMatrix[3] =
    this->hardCameraRotation[3] * tempPose.rotationMatrix[0] +
    this->hardCameraRotation[4] * tempPose.rotationMatrix[3] +
    this->hardCameraRotation[5] * tempPose.rotationMatrix[6];
  pose.rotationMatrix[4] =
    this->hardCameraRotation[3] * tempPose.rotationMatrix[1] +
    this->hardCameraRotation[4] * tempPose.rotationMatrix[4] +
    this->hardCameraRotation[5] * tempPose.rotationMatrix[7];
  pose.rotationMatrix[5] =
    this->hardCameraRotation[3] * tempPose.rotationMatrix[2] +
    this->hardCameraRotation[4] * tempPose.rotationMatrix[5] +
    this->hardCameraRotation[5] * tempPose.rotationMatrix[8];
  pose.rotationMatrix[6] =
    this->hardCameraRotation[6] * tempPose.rotationMatrix[0] +
    this->hardCameraRotation[7] * tempPose.rotationMatrix[3] +
    this->hardCameraRotation[8] * tempPose.rotationMatrix[6];
  pose.rotationMatrix[7] =
    this->hardCameraRotation[6] * tempPose.rotationMatrix[1] +
    this->hardCameraRotation[7] * tempPose.rotationMatrix[4] +
    this->hardCameraRotation[8] * tempPose.rotationMatrix[7];
  pose.rotationMatrix[8] =
    this->hardCameraRotation[6] * tempPose.rotationMatrix[2] +
    this->hardCameraRotation[7] * tempPose.rotationMatrix[5] +
    this->hardCameraRotation[8] * tempPose.rotationMatrix[8];

  //------------------------------- Tmo = Tco + Rco x Tmc
  pose.x = this->hardCameraTranslation[0] +
    this->hardCameraRotation[0] * tempPose.x +
    this->hardCameraRotation[1] * tempPose.y +
    this->hardCameraRotation[2] * tempPose.z;
  pose.y = this->hardCameraTranslation[1] +
    this->hardCameraRotation[3] * tempPose.x +
    this->hardCameraRotation[4] * tempPose.y +
    this->hardCameraRotation[5] * tempPose.z;
  pose.z = this->hardCameraTranslation[2] +
    this->hardCameraRotation[6] * tempPose.x +
    this->hardCameraRotation[7] * tempPose.y +
    this->hardCameraRotation[8] * tempPose.z;
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

boost::array<double, 9>
AbstractCamera::getRotationMatrix(const scene::AnyHardCamera& anyHardCamera)
{
  return boost::apply_visitor(scene::GetHardCamera(),
                              anyHardCamera).rotationMatrix;
}

boost::array<double, 3>
AbstractCamera::getTranslationVector(const scene::AnyHardCamera& anyHardCamera)
{
  return boost::apply_visitor(scene::GetHardCamera(),
                              anyHardCamera).translationVector;
}
