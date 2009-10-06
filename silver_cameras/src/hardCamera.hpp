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

#ifndef _HARDCAMERA_H_
#define _HARDCAMERA_H_

#include <stdexcept>
#include <string>

#include <boost/noncopyable.hpp>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "scene.hpp"

/** Base class to all classes which manage camera hardware.
 * All hardware camera class must fill the following requeriments:
 *   -# Only one class may be instantiated for each real (physical) camera
 *   -# Multiple objects may hold one same hardCamera instance
 *   -# Each of that objects may be running in one thread. ie: Multiples
 *      threads may access one same hardCamera instance
 *   -# Each call to captureFrame method must return only a frame still unread
 *      by calling object
 */
class HardCamera : public boost::noncopyable
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

  /** Add a client to hardCamera.
   * This function add a client position in unreadImage vector.
   * @return The uid of new client.
   */
  unsigned addClient();

  /** Return an IplImage ready to receive the captured frames.
   * This method won't free a supposed IplImage pointed by iplImage parameter.
   * The iplImage is supposed pointing to null, and the new IplImage allocated
   * will be assigned to it.
   * @param iplImage A pointer to pointer to IplImage, which will point to
   *                 created image
   */
  void createIplImage(IplImage** iplImage) const;

  virtual void saveFrame() = 0;

  /** Return a still not read frame.
   * If the last grabed frame was already read by calling object, the calling
   * thread will be locked until a new frame.
   * @param image A pointer to pointer to an IplImage where the new frame will
   *              be returned.
   * @param clientUid The unique id of calling object.
   */
  virtual void captureFrame(IplImage** image, unsigned clientUid) = 0;

  /** Call the captureFrame method and rectify the captured frame.
   * @param image A pointer to pointer to an IplImage where the new frame will
   *              be returned.
   * @param clientUid The unique id of calling object.
   */
  void captureRectFrame(IplImage** image, unsigned clientUid);

protected:

  HardCamera(const scene::Camera& config);

  /// A string representing the uid of the hardware camera.
  /// It must be numbers in decimal base.
  const std::string uid;

  // Frame size measures in pixels.
  const unsigned frameSize;
  const unsigned frameWidth, frameHeight;

  /// Each bool represents if one client read or not the last
  /// received image from camera. Each client have one unique id,
  /// which is it position on vector.
  std::vector<bool> unreadImage;

  const float frameRate;

  void undistortFrame(IplImage* frame);

private:

  friend class HardCameraFactory;

  /// Maps to distort captured images
  IplImage* mapx;
  IplImage* mapy;

  virtual void initialize() = 0;
};

#endif /* _HARDCAMERA_H_ */
