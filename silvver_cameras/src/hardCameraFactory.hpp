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
#include <boost/tuple/tuple.hpp>
#include <boost/variant.hpp>
#include <map>
#include <string>

#include "hardCameras/hardCamera.hpp"
#include "hardCameras/hardCameraDescriptions.hpp"

class HardCameraFactory : public boost::noncopyable
{
public:

  /** Return a shared_ptr to the hardCamera described in cameraConfig and uid.
   * Return the shared_ptr and the client uid related to that hardware camera.
   * @param cameraConfig The struct describing the hardware camera.
   * @return A tuple composed by hardCamera and clientUid.  HardCamera is a
   *         shared_ptr which will point to the required hardware camera, and
   *         the clientUid is an unsigned with the uid of calling object.
   */
   static boost::tuple<boost::shared_ptr<HardCamera>, unsigned>
   create(const scene::AnyHardwareCamera& cameraConfig);

private:

  struct ConstructHardCamera : public boost::static_visitor<HardCamera*>
  {
    HardCamera* operator()(const scene::PseudoCamera& config) const;
    HardCamera* operator()(const scene::DC1394& config) const;
  };

  HardCameraFactory();

  /// The std::string key is the HardCamera identifier.
  static std::map< std::string, boost::shared_ptr<HardCamera> >
  createdHardCameras;

  static boost::mutex mutexCameraCreate;
};

#endif /* _HARD_CAMERA_FACTORY_HPP_ */
