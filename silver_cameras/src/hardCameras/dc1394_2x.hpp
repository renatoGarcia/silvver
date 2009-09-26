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

#ifndef _DC1394_2X_HPP_
#define _DC1394_2X_HPP_

#include "../hardCamera.hpp"

#include <boost/thread/mutex.hpp>

#include <dc1394/dc1394.h>

class DC1394: public HardCamera
{
public:

  DC1394(const scene::Camera& config);

  ~DC1394();

  void initialize();

  void captureFrame(IplImage** iplImage);

  // Grava a última imagem da câmera no disco
  void saveFrame();

private:

  static const int N_BUFFERS = 4;

  // Convert the HardCamera frameWidth and frameHeight attributes to an
  // equivalent dc1394video_mode_t
  dc1394video_mode_t getDc1394VideoMode();

  // Convert the HardCamera frameRate to an equivalent dc1394framerate_t
  dc1394framerate_t getDc1394FrameRate();

  // dc1394 context
  dc1394_t* context;

  dc1394camera_t* camera;

  boost::mutex mutexCaptureFrame;
};

#endif /* _DC1394_2X_HPP_ */
