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

/** \file scene.hpp
 * \brief This file contain all structs with the configuration of scene.
 * In this structs be put all data read from lua config file.
 */

#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include <boost/array.hpp>
#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>
#include <vector>

#include "hardCameras/hardCameraDescriptions.hpp"
#include "abstractCameras/targetDescriptions.hpp"

namespace scene
{
  /** Struct with all configurations of a camera.
   * This struct groups the hardware camera specific configurations and the
   * commons camera extrinsic parameters.
   */
  struct Camera
  {
    VariantHardwareCamera hardware;

    /// Translation vector of camera, in origin referential.
    boost::array<double, 3> translationVector;

    /** Rotation matrix of camera, in origin referential.
     * The format must be {U11, U12, U13, U21, U22, U23, U31, U32, U33},
     * where Uxy is the item of row x and column y. */
    boost::array<double, 9> rotationMatrix;
  };

  /// Complete description of scene, with cameras and targets.
  struct Scene
  {
    /// Tuple with configuration structs of all targets in scene.
    boost::tuple< boost::optional<ArtkpTargets> > targets;

    /// Vector with configuration structs of all camera in scene.
    std::vector<Camera> cameras;
  };
}
#endif /* _SCENE_HPP_ */
