/* Copyright 2009,2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#ifndef _ABSTRACT_CAMERA_HPP_
#define _ABSTRACT_CAMERA_HPP_

#include <boost/asio/io_service.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <cstdlib>
#include <stdint.h>

#include "../exceptions.hpp"
#include "../frame.hpp"
#include "../hardCameras/hardCamera.hpp"
#include "../log.hpp"
#include "../observer.hpp"
#include "../scene.hpp"
#include "common/connection/channel.ipp"
#include "common/processorOptions.hpp"
#include "common/silvverTypes.hpp"
#include "common/serializations.hpp"

/// Abstract base class to all abstract cameras.
class AbstractCamera
  :public boost::noncopyable
  ,public Observer
{
public:
  virtual ~AbstractCamera();

  /// Update the status of incoming image to available.
  /// This mothod is called by Subject class.
  virtual void update();

  /// Start the abstractCamera target finding.
  virtual void run() = 0;

  /// Stop the abstractCamera.
  virtual void stop() = 0;

private:
  /// Hardware camera which provides images.
  boost::shared_ptr<HardCamera> subjectHardCamera;

protected:

  AbstractCamera(const scene::Camera& cameraConfig,
                 const unsigned targetSetUid,
                 const procOpt::AnyProcOpt& procOptions);

  /// Update the frame in currentFrame.
  void updateFrame();

  template<class TargetType>
  void sendLocalizations(const std::vector<silvver::Identity<TargetType> >& localizations);

  /// Tranform a pose in camera coordinates do world coordinates.
  void toWorld(silvver::Pose& pose) const;

  Frame currentFrame;

  const silvver::AbstractCameraUid abstractCameraUid;

private:
  static
  connection::Channel* createChannel(const connection::TcpIpEp& receptionistEp);

  static boost::asio::io_service ioService;

  /// Channel with the silvver-server used to send the target localizations.
  boost::scoped_ptr<connection::Channel> serverChannel;

  /// This boolean represents if the last image grabbed by hardCamera was
  /// already precessed or not. Its access is thread safe.
  bool unreadImage;

  /// Controls the access to unreadImage attribute.
  boost::mutex unreadImageAccess;

  /// Condition to wait for an unread image from hardCamera.
  boost::condition_variable unreadImageCondition;

  /// Rotation matrix of camera in world coordinates.
  const boost::array<double, 9> rot;

  /// Translation vector of camera in world coordinates.
  const boost::array<double, 3> trans;
};

template<class TargetType>
void
AbstractCamera::sendLocalizations(const std::vector<silvver::Identity<TargetType> >& localizations)
{
  silvver::CameraReading<TargetType>
    cameraReading(this->abstractCameraUid,
                  this->currentFrame.timestamp,
                  localizations);

  try
  {
    this->serverChannel->send(cameraReading);
  }
  catch (const server_connection_error& e)
  {
    message(LogLevel::ERROR)
      << "[Fatal error]\n" << errorsInfo2string(e) << std::endl;
    exit(EXIT_FAILURE);
  }
}

#endif /* _ABSTRACT_CAMERA_HPP_ */
