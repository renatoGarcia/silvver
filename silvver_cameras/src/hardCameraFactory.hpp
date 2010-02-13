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

#ifndef _HARD_CAMERA_FACTORY_HPP_
#define _HARD_CAMERA_FACTORY_HPP_

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <map>
#include <string>

#include "hardCameras/hardCamera.hpp"
#include "hardCameras/hardCameraDescriptions.hpp"

class HardCameraFactory : public boost::noncopyable
{
public:
  /// Return a shared_ptr to the hardCamera described in cameraConfig.
  /// @param cameraConfig The struct describing the hardware camera.
  /// @return A shared_ptr which will point to the required hardware camera.
   static boost::shared_ptr<HardCamera>
   create(const scene::AnyHardwareCamera& cameraConfig);

private:
  struct ConstructHardCamera : public boost::static_visitor<HardCamera*>
  {
    HardCamera* operator()(const scene::PseudoCamera& config) const;
    HardCamera* operator()(const scene::DC1394& config) const;
    HardCamera* operator()(const scene::V4l2& config) const;
  };

  HardCameraFactory();

  /// The std::string key is the HardCamera identifier.
  static std::map< std::string, boost::shared_ptr<HardCamera> >
  createdHardCameras;

  static boost::mutex mutexCameraCreate;
};

#endif /* _HARD_CAMERA_FACTORY_HPP_ */
