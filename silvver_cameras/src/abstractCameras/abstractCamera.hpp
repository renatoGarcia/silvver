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
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <string>

#include <opencv/cv.h>

#include "../connection.hpp"
#include "../hardCameras/hardCamera.hpp"
#include "../scene.hpp"
#include <silvverTypes.hpp>

/// Abstract base class to all abstract cameras.
class AbstractCamera : boost::noncopyable
{
public:

  /// Start a new thread and run the operator() method.
  void run();

  /// Stop the operator() method and the thread.
  void stop();

  virtual ~AbstractCamera();

  /// It's the method which will be called when a new thread was created, and
  /// will make all camera work.
  virtual void operator()()=0;

protected:

  IplImage* currentFrame;

  /// Connection with the silvver-server used to send the target localizations.
  const boost::shared_ptr<Connection> serverConnection;

  /** When this variable become True, the loop in operator() method implemented
   *  by a derivate class must exit */
  bool stopping;

  AbstractCamera(const scene::Camera& cameraConfig,
                 boost::shared_ptr<Connection> connection,
                 bool showImage=false);

  // Atualiza a imagem em "*imgCamera", e retorna o instante no tempo em que elas foram capturadas.
  // Este tempo é dado em milisegundos, e é calculado levando em conta o "tempoInicial".
  void updateFrame();

  /// Tranform a pose in camera coordinates do world coordinates.
  void toWorld(silvver::Pose& pose) const;

private:

  boost::scoped_ptr<boost::thread> runThread;

  /// Hardware camera which provides images, and the uid of this abstract
  /// camera related to that hardCamera.
  const boost::tuple<boost::shared_ptr<HardCamera>, unsigned> hardCamera;

  /// Rotation matrix of camera in world coordinates.
  boost::array<double, 9> rot;

  /// Translation vector of camera in world coordinates.
  boost::array<double, 3> trans;

  // Número de imagens processadas, usado para calcular a taxa média
  // de quadros por segundo.
  unsigned frameCounter;
  float frameRate;

  const bool showImage;
  const std::string windowName;
};

#endif
