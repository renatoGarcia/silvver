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

#ifndef _DC1394_1X_HPP_
#define _DC1394_1X_HPP_

#include "../hardCamera.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <string>

#include <libdc1394/dc1394_control.h>
#include <libraw1394/raw1394.h>

#include "conversions.h"
#include "../scene.hpp"

class DC1394: public HardCamera
{
public:

  DC1394(const scene::DC1394& config);

  ~DC1394();

  void initialize();

  void captureFrame(IplImage** iplImage, unsigned clientUid);

private:

  void runCapturer();

  void findThisCamera(nodeid_t& node, int& cardIndex);

  std::string findVideo1394Device(unsigned cardNumber);

  // Convert the HardCamera frameRate to an equivalent DC1394 frame rate
  int getDc1394FrameRate();

  static const int NUMERO_BUFFERS = 4;

  /// A string representing the uid of the camera.
  /// It must be numbers in decimal base.
  const std::string uid;

  const float frameRate;

  raw1394handle_t raw1394Handle;

  dc1394_cameracapture dc1394Camera;
  bool                 bDc1394CameraCreated;

  unsigned char* currentFrame;
  unsigned char* frameBuffer[2];

  /// Camera frame bytes per pixel
  unsigned bytesPerPixel;

  int mode;

  bayer_pattern_t pattern;

  boost::shared_mutex bufferAccess;
  boost::condition_variable_any unreadFrameCondition;

  boost::scoped_ptr<boost::thread> grabFrameThread;
};

#endif /* _DC1394_1X_HPP_ */
