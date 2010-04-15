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

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include "../connection.hpp"
#include "../hardCameras/hardCamera.hpp"
#include "../frame.hpp"
#include "../observer.hpp"
#include "../scene.hpp"

/// Abstract base class to all abstract cameras.
class AbstractCamera: public boost::noncopyable, public Observer
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
                 boost::shared_ptr<Connection> connection);

  /// Update the frame in currentFrame.
  void updateFrame();

  /// Tranform a pose in camera coordinates do world coordinates.
  void toWorld(silvver::Pose& pose) const;

  Frame currentFrame;

  /// Connection with the silvver-server used to send the target localizations.
  const boost::shared_ptr<Connection> serverConnection;

private:
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

#endif /* _ABSTRACT_CAMERA_HPP_ */
