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

#include <boost/shared_ptr.hpp>
#include <fstream>
#include <string>
#include <vector>

#include <ARToolKitPlus/TrackerSingleMarker.h>

#include "abstractCamera.hpp"
#include "targetDescriptions.hpp"

class ArtkpCamera  : public AbstractCamera
{
public:

  class initialize_error : public std::logic_error
  {
  public:
    initialize_error(const std::string& what)
      :logic_error(what)
    {}
  };

  class detect_marker_error : public std::logic_error
  {
  public:
    detect_marker_error(const std::string& what)
      :logic_error(what)
    {}
  };

  ArtkpCamera(const scene::Camera& cameraConfig,
              const scene::ArtkpTargets& targets,
              boost::shared_ptr<Connection> connection,
              bool showImages);

  ~ArtkpCamera();

  virtual void operator()();

private:

  void initialize();

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

  std::string camConfigFileName;

  /// Width of pattern, counting with border. [mm]
  const int patternWidth;

  MyLogger logger;

  const boost::scoped_ptr<ARToolKitPlus::TrackerSingleMarker> tracker;

  std::vector<std::string> vecDescriptionFilePath;

  /// Translates the inner artkp id to silvver id
  boost::array<int, MAX_TARGETS> idMap;
};

#endif /* _ARTKP_CAMERA_HPP_ */
