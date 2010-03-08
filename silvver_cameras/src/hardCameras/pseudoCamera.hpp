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

#ifndef _PSEUDO_CAMERA_HPP_
#define _PSEUDO_CAMERA_HPP_

#include "hardCamera.hpp"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>

class PseudoCamera
  :public HardCamera
{
public:

  PseudoCamera(const scene::PseudoCamera& config);

  ~PseudoCamera();

private:

  static const unsigned BITS_PER_PIXEL = 8;

  void doWork();

  const boost::filesystem::path path;

  const float frameRate;

  boost::filesystem::directory_iterator dirIterator;
  const boost::filesystem::directory_iterator  endIterator;

  /// Delay time to simulate the correct fps
  const boost::posix_time::millisec delay;

  boost::scoped_ptr<boost::thread> grabFrameThread;
};

#endif /* _PSEUDO_CAMERA_HPP_ */
