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

#ifndef _HARDCAMERA_HPP_
#define _HARDCAMERA_HPP_

#include <boost/array.hpp>
#include <boost/format.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <string>
#include <vector>

#include <opencv/cv.h>

#include "../frame.hpp"
#include "../observer.hpp"
#include "hardCameraDescriptions.hpp"
#include "timestampMap.hpp"

/** Base class to all classes which manage camera hardware. !!!!OUTDATED!!!!!
 * All hardware camera class must fill the following requeriments:
 *   -# Only one class may be instantiated for each real (physical) camera
 *   -# Multiple objects may hold one same hardCamera instance
 *   -# Each of that objects may be running in one thread. ie: Multiples
 *      threads may access one same hardCamera instance
 *   -# Each call to captureFrame method must return only a frame still unread
 *      by calling object
 */
class HardCamera: public boost::noncopyable, public Subject
{
public:
  virtual ~HardCamera();

  void getDistortedFrame(Frame& image);

  void getUndistortedFrame(Frame& image);

  /// A uid to differentiate this camera, e.g.: when saving images or
  /// in title of window where showing the captured images.
  const unsigned silvverUid;

protected:
  HardCamera(const scene::Hardware& config, int iplDepth);

  /// This method must be called by derived classes when it read a new frame
  void updateCurrentFrame(Frame& frame);

  /// Frame size measures in pixels.
  const unsigned framePixels;
  const CvSize frameSize;

  /// Image depth in IPL format.
  const int iplDepth;

private:
  friend class HardCameraFactory;

private:
  static boost::format createFormat(const std::string& strFormat,
                                    const unsigned silvverUid);

  unsigned getBitsPerPixel(int iplDepth) const;

  Frame* distortedFrame;
  Frame* undistortedFrame;

  boost::array<Frame, 2> undistortedFrameBuffer;

  /// Mutex to control the read/write operations in distortedFrame and
  /// undistortedFrame.
  boost::shared_mutex framesAccessMutex;

  /// Maps to distort captured images
  IplImageWrapper mapx;
  IplImageWrapper mapy;

  const bool showImages;
  const std::string windowName;

  const bool saveDistortedImages;
  const bool saveUndistortedImages;
  boost::format saveImageFormat;
  unsigned imagesCounter;

  const bool saveTimestamp;
  TSMapWriter timestampWriter;
};

#endif /* _HARDCAMERA_HPP_ */
