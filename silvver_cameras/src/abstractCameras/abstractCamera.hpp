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

#ifndef _ABSTRACT_CAMERA_HPP_
#define _ABSTRACT_CAMERA_HPP_

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <string>

#include <opencv/cv.h>

#include "../connection.hpp"
#include "../hardCameras/hardCamera.hpp"
#include "../scene.hpp"

/// Abstract base class to all abstract cameras.
class AbstractCamera : boost::noncopyable
{
public:

  /// Start the abstractCamera target finding.
  virtual void run()=0;

  /// Stop the abstractCamera.
  virtual void stop()=0;

  virtual ~AbstractCamera();

protected:

  AbstractCamera(const scene::Camera& cameraConfig,
                 boost::shared_ptr<Connection> connection,
                 bool showImage=false);

  /// Update the frame pointed by currentFrame.
  void updateFrame();

  /// Tranform a pose in camera coordinates do world coordinates.
  void toWorld(silvver::Pose& pose) const;

  IplImage* currentFrame;

  /// Connection with the silvver-server used to send the target localizations.
  const boost::shared_ptr<Connection> serverConnection;

private:

  /// Hardware camera which provides images, and the uid of this abstract
  /// camera related to that hardCamera.
  const boost::tuple<boost::shared_ptr<HardCamera>, unsigned> hardCamera;

  /// Rotation matrix of camera in world coordinates.
  const boost::array<double, 9> rot;

  /// Translation vector of camera in world coordinates.
  const boost::array<double, 3> trans;

  // Número de imagens processadas, usado para calcular a taxa média
  // de quadros por segundo.
  unsigned frameCounter;
  float frameRate;

  const bool showImage;
  const std::string windowName;
};

#endif
