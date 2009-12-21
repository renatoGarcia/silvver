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
#include <boost/thread/shared_mutex.hpp>
#include <stdexcept>
#include <string>
#include <vector>

#include <opencv/cv.h>

#include "hardCameraDescriptions.hpp"
#include "../observer.hpp"

/** Base class to all classes which manage camera hardware.
 * All hardware camera class must fill the following requeriments:
 *   -# Only one class may be instantiated for each real (physical) camera
 *   -# Multiple objects may hold one same hardCamera instance
 *   -# Each of that objects may be running in one thread. ie: Multiples
 *      threads may access one same hardCamera instance
 *   -# Each call to captureFrame method must return only a frame still unread
 *      by calling object
 */
class HardCamera
  :public boost::noncopyable
  ,public Subject
{
public:

  class camera_parameter_error : public std::logic_error
  {
  public:
    camera_parameter_error(const std::string& whatArg)
      :logic_error(whatArg){}
  };

  class open_camera_error : public std::runtime_error
  {
  public:
    open_camera_error(const std::string& whatArg)
      :runtime_error(whatArg){}
  };

  class capture_image_error : public std::runtime_error
  {
  public:
    capture_image_error(const std::string& whatArg)
      :runtime_error(whatArg){}
  };

  virtual ~HardCamera();

  /** Return an IplImage ready to receive the captured frames.
   * This method won't free a supposed IplImage pointed by iplImage parameter.
   * The iplImage is supposed pointing to null, and the new IplImage allocated
   * will be assigned to it.
   * @param iplImage A pointer to pointer to IplImage, which will point to
   *                 created image
   */
  void createIplImage(IplImage** iplImage) const;

  void getDistortedFrame(IplImage** image);// const;

  void getUndistortedFrame(IplImage** image);// const;

protected:

  HardCamera(const scene::Hardware& config, unsigned bitsPerPixel);

  /// This method must be called by derived classes when it read a new frame
  void updateCurrentFrame(IplImage* frame);

  /// The IplImages must have color channels in bgr order, this function
  /// transform a wrong rgb IplImage in a bgr IplImage. The input and output
  /// cannot point to the same IplImage.
  void fixChannelOrder(const IplImage* const input, IplImage* const output);

  /// Frame size measures in pixels.
  const unsigned frameSize;
  const unsigned frameWidth, frameHeight;

  /// Camera frame bits per pixel
  const unsigned bitsPerPixel;

private:

  friend class HardCameraFactory;

  /// Swap the channels red and blue. The input and output cannot point to
  /// the same image.
  template <class T>
  void rgb2bgr(const T* const input, T* const output, const unsigned nPixels);

  virtual void initialize() = 0;

  IplImage* distortedFrame;
  IplImage* undistortedFrame;

  IplImage* undistortedFrameBuffer[2];

  /// A string to differentiate this camera, e.g.: when saving images or
  /// in title of window where showing the captured images.
  const std::string cameraIdentifier;

  /// Mutex to control the read/write operations in distortedFrame and
  /// undistortedFrame.
  boost::shared_mutex framesAccessMutex;

  /// Maps to distort captured images
  IplImage* mapx;
  IplImage* mapy;

  const bool showImages;
  const std::string windowName;

  const bool saveDistortedImages;
  const bool saveUndistortedImages;
  boost::format saveImageFormat;
  unsigned imagesCounter;
};

#endif /* _HARDCAMERA_HPP_ */
