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

#include <string>

#include <boost/thread/mutex.hpp>

#include <libdc1394/dc1394_control.h>
#include <libraw1394/raw1394.h>
#include <sys/time.h>

#include "conversions.h"

class DC1394: public HardCamera
{
public:

  enum Format
  {
    F_VGA_NONCOMPRESSED = FORMAT_VGA_NONCOMPRESSED
  };

  DC1394(const scene::Camera& config,
         Format format = F_VGA_NONCOMPRESSED);

  ~DC1394();

  void initialize();

  void captureFrame(IplImage** iplImage);

  // Grava a última imagem da câmera no disco
  void saveFrame();

private:

  static const int NUMERO_BUFFERS = 4;

  const int nCard;
  const std::string device;

  raw1394handle_t raw1394Handle;
  bool            bRaw1394HandleCreated;

  dc1394_cameracapture dc1394Camera;
  bool                 bDc1394CameraCreated;

  Format format;
  int mode;
  int bytesPerPixel;

  bayer_pattern_t pattern;

  boost::mutex mutexCaptureFrame;
};

#endif /* _DC1394_1X_HPP_ */
