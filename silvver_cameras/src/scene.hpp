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

/// \file scene.hpp
/// \brief This file contain all structs with the configuration of scene.
/// In this structs be put all data read from lua config file.

#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include <vector>

#include "abstractCameras/targetDescriptions.hpp"
#include "hardCameras/hardCameraDescriptions.hpp"

namespace scene{

/// Complete description of scene, with hardCameras and targetSets.
struct Scene
{
  std::vector<AnyTargetSet> targetSets;

  /// Vector with configuration structs of all camera in scene.
  std::vector<AnyHardCamera> hardCameras;
};

}

#endif /* _SCENE_HPP_ */
