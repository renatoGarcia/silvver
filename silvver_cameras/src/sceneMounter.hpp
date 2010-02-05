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

#ifndef _SCENE_MOUNTER_HPP_
#define _SCENE_MOUNTER_HPP_

#include <boost/ptr_container/ptr_vector.hpp>
#include <string>

#include "abstractCameras/abstractCamera.hpp"
#include "scene.hpp"

class SceneMounter
{
public:

  /** sceneMounter class constructor.
   *
   * @param serverIP IP address where silvver-server is running.
   * @param receptionistPort Port where the receptionist of silvver-server
   *                         is hearing.
   * @param sceneDescriptorFile Path to the lua file scene descriptor.
   */
  SceneMounter(const std::string& serverIp, const int receptionistPort,
               const std::string& sceneDescriptorFile);

  ~SceneMounter();

  /** Initialize all abstract cameras.
   *
   * The abstract cameras that will be initized are described in the scene
   * descriptor lua file.
   */
  void mount();

private:

  const std::string sceneDescriptorFile;

  const std::string serverIp;
  const unsigned receptionistPort;

  /// Initialize the abstract camera given by cameraConfig and targets.
  void constructAbstractCamera(const scene::Camera& cameraConfig,
                               const scene::AnyTarget& targets);

  /// Handle all opened abstract cameras.
  boost::ptr_vector<AbstractCamera> abstractCameras;
};

#endif /* _SCENE_MOUNTER_HPP_ */