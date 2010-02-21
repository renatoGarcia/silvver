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

#include <boost/format.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <exception>
#include <string>
#include <vector>

#include <opencv/cv.h>

#include "../iplImageWrapper.hpp"
#include "../observer.hpp"
#include "hardCameraDescriptions.hpp"

/** Base class to all classes which manage camera hardware.
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

  class hard_camera_error: public std::exception
  {
  public:
    hard_camera_error(const std::string& whatArg) :msg(whatArg) {}
    ~hard_camera_error() throw() {}
    const char* what() const throw() { return msg.c_str(); }
  private:
    const std::string msg;
  };

  class open_camera_error: public hard_camera_error
  {
  public:
    open_camera_error(const std::string& whatArg)
      :hard_camera_error(whatArg){}
  };

  /// Threw when a parameter or feature is invalid
  class camera_parameter_error: public hard_camera_error
  {
  public:
    camera_parameter_error(const std::string& whatArg)
      :hard_camera_error(whatArg){}
  };

  class capture_image_error: public hard_camera_error
  {
  public:
    capture_image_error(const std::string& whatArg)
      :hard_camera_error(whatArg){}
  };

  virtual ~HardCamera();

  IplImageWrapper::IplParameters getImageParameters() const;

  void getDistortedFrame(IplImageWrapper& image);

  void getUndistortedFrame(IplImageWrapper& image);

protected:

  HardCamera(const scene::Hardware& config, int iplDepth, int nChannels);

  /// This method must be called by derived classes when it read a new frame
  void updateCurrentFrame(boost::shared_ptr<IplImageWrapper> frame);

  /// The IplImages must have color channels in bgr order, this function
  /// transform a wrong rgb IplImage in a bgr IplImage. The input and output
  /// cannot point to the same IplImage.
  // void fixChannelOrder(const IplImage* const input, IplImage* const output);

  /// Frame size measures in pixels.
  const unsigned framePixels;
  const CvSize frameSize;

  /// Number of channels in image.
  const int nChannels;

  /// Image depth in IPL format.
  const int iplDepth;

private:

  friend class HardCameraFactory;

  unsigned getBitsPerPixel(int iplDepth) const;

  // /// Swap the channels red and blue. The input and output cannot point to
  // /// the same image.
  // template <class T>
  // void rgb2bgr(const T* const input, T* const output, const unsigned nPixels);

  virtual void initialize() = 0;

  boost::shared_ptr<IplImageWrapper> distortedFrame;
  boost::shared_ptr<IplImageWrapper> undistortedFrame;

  boost::shared_ptr<IplImageWrapper> undistortedFrameBuffer[2];

  /// A string to differentiate this camera, e.g.: when saving images or
  /// in title of window where showing the captured images.
  const std::string cameraIdentifier;

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
};

#endif /* _HARDCAMERA_HPP_ */
