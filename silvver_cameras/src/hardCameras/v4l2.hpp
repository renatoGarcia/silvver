/* Copyright 2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#ifndef _V4L2_HPP_
#define _V4L2_HPP_

#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <map>

#include <string>
#include <string.h>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <stdint.h>

#include "colorConverter.hpp"
#include "hardCamera.hpp"

class V4L2: public HardCamera
{
public:

  V4L2(const scene::V4l2& config);

  ~V4L2() throw();

private:

  /// Map to color space 
  typedef std::map<std::string,boost::tuple<__u32,ColorSpace> > ColorFormatMap;

  static const int N_BUFFERS = 4;

  static const ColorFormatMap COLOR_FORMAT;

  void doWork();

  /// Find the device path bounded to this v4l2 camera.
  /// @return A string with the device path.
  std::string findDevice() const;

  ColorConverter createColorConverter(const scene::V4l2& config);

  void setFeatures(const scene::V4l2& config);

  void setFormat(const scene::V4l2& config);

  const unsigned uid;

  const int cameraFd;

  const unsigned width;

  const unsigned height;

  boost::scoped_ptr<boost::thread> grabFrameThread;

  struct Buffers
  {
    uint8_t* start;
    size_t length;
  } buffers[N_BUFFERS];

  ColorConverter colorConverter;
};

#endif /* _V4L2_HPP_ */
