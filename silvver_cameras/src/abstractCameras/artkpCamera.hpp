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

#ifndef _ARTKP_CAMERA_HPP_
#define _ARTKP_CAMERA_HPP_

#include <boost/array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <stdexcept>
#include <string>
#include <vector>

#include <ARToolKitPlus/TrackerSingleMarker.h>

#include "abstractCamera.hpp"
#include "mountedTarget.hpp"
#include "targetDescriptions.hpp"

class ArtkpCamera: public AbstractCamera, private MountedTarget
{
public:

  ArtkpCamera(const scene::Camera& cameraConfig,
              const scene::ArtkpTargets& targets);

  ~ArtkpCamera();

  virtual void run();

  virtual void stop();

private:

  /// It's the method which will be called when a new thread was created, and
  /// will make all camera work.
  void doWork();

  class MyLogger : public ARToolKitPlus::Logger
  {
  public:

    void artLog(const char* const nStr)
    {
      this->message = nStr;
    }

    std::string message;
  };

  static const int MAX_TARGETS = 40;

  /// Width of pattern, counting with border. [mm]
  const int patternWidth;

  /// Grey level threshold between black and white.
  const int threshold;

  MyLogger logger;

  const boost::scoped_ptr<ARToolKitPlus::TrackerSingleMarker> tracker;

  /// Translates the inner artkp id to silvver id
  boost::array<int, MAX_TARGETS> idMap;

  boost::scoped_ptr<boost::thread> runThread;
};

#endif /* _ARTKP_CAMERA_HPP_ */
