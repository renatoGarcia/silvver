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

#include <boost/scoped_ptr.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/thread.hpp>

#include <dc1394/dc1394.h>

class DC1394: public HardCamera
{
public:

  DC1394(const scene::DC1394& config);

  ~DC1394();

  void initialize();

  void captureFrame(IplImage** iplImage, unsigned clientUid);

private:

  void runCapturer();

  static const int N_BUFFERS = 4;

  /// A string representing the uid of the camera.
  /// It must be numbers in decimal base.
  const std::string uid;

  const float frameRate;

  // Convert the HardCamera frameWidth and frameHeight attributes to an
  // equivalent dc1394video_mode_t
  dc1394video_mode_t getDc1394VideoMode(const std::string& colorMode) const;

  unsigned getBitsPerPixel(const std::string& colorMode) const;

  // Convert the HardCamera frameRate to an equivalent dc1394framerate_t
  dc1394framerate_t getDc1394FrameRate() const;

  void setFeatures();

  const dc1394video_mode_t videoMode;

  /// Camera frame bits per pixel
  const unsigned bitsPerPixel;

  /// Frame buffer size in bytes
  const unsigned bufferSize;

  // dc1394 context
  dc1394_t* context;

  dc1394camera_t* camera;

  /// Pointer to the last captured frame.
  dc1394video_frame_t* currentFrame;
  dc1394video_frame_t* frameBuffer[2];

  boost::shared_mutex bufferAccess;
  boost::condition_variable_any unreadFrameCondition;

  boost::scoped_ptr<boost::thread> grabFrameThread;

  const std::string brightness;
  const std::string exposure;
  const boost::array<std::string, 2> whiteBalance;
  const std::string shutter;
  const std::string gain;
};

#endif /* _DC1394_2X_HPP_ */
