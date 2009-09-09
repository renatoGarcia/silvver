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

#ifndef HARD_CAMERA_HPP
#define HARD_CAMERA_HPP

#include <stdexcept>
#include <string>

#include <boost/noncopyable.hpp>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "scene.hpp"

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

  void createIplImage(IplImage** iplImage) const;

  virtual void saveFrame() = 0;

  virtual void captureFrame(IplImage** imgRetorno) = 0;

protected:

  HardCamera(const scene::Camera& config);

  // A string representing the uid of the hardware camera.
  // It must be numbers in decimal base.
  const std::string uid;

  // Frame size measures in pixels.
  const unsigned frameSize;
  const unsigned frameWidth, frameHeight;

  const float frameRate;

  void undistortFrame(IplImage* frame);

private:

  friend class HardCameraFactory;

  /// Maps to distort captured images
  IplImage* mapx;
  IplImage* mapy;

  virtual void initialize() = 0;
};

#endif
