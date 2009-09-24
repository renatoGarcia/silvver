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

#include <vector>
#include <string>

#include <boost/ptr_container/ptr_vector.hpp>

#include "abstractCamera.hpp"
#include "connection.hpp"
#include "scene.hpp"

class SceneMounter
{
public:

  /** sceneMounter class constructor.
   *
   * @param receptionistPort Port where the receptionist of silver-server is hearing.
   * @param serverIP IP address where silver-server is running.
   * @param sceneDescriptorFile Path to XML file scene descriptor.
   */
  SceneMounter(const std::string& serverIp, const int receptionistPort,
               const std::string& sceneDescriptorFile);

  /** Initialize all abstract cameras.
   *
   * The abstract cameras that will be initized are described in the scene
   * descriptor XML file.
   */
  void mount(const bool showImages);

  /// Close all opened abtract cameras
  void dismount();

private:

  const std::string sceneDescriptorFile;

  const std::string serverIp;
  const unsigned receptionistPort;

  // Initialize the abstract camera given by cameraConfig
  void constructAbstractCamera(const scene::Camera& cameraConfig,
                               const scene::VariantAnyTarget& targets,
                               const std::string& targetType,
                               const bool showImages);

  boost::ptr_vector<AbstractCamera> abstractCameras;
};

#endif /* _SCENE_MOUNTER_HPP_ */
