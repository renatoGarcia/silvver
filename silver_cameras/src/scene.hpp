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

#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include <map>
#include <string>
#include <vector>

#include <boost/array.hpp>
#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/variant.hpp>

namespace scene
{
  struct Camera
  {
    float frameRate;

    /// Type of hardware camera.
    std::string hardware;

    /// Device where the camera is mapped,
    /// or path to input images for pseudoCamera
    std::string device;

    /// String representation of camera unique identifier in decimal base.
    std::string uid;

    boost::array<unsigned, 2> resolution;

    /// {fx, fy}.
    boost::array<double, 2> focalLength;

    /// {cx, cy}.
    boost::array<double, 2> principalPoint;

    /// Radial distortion coefficients {k1, k2, k3}.
    boost::array<double, 3> radialCoef;

    /// Tangential distortion coefficients {p1, p2}.
    boost::array<double, 2> tangentialCoef;

    /// Translation vector of camera, in origin referential.
    boost::array<double, 3> translationVector;

    /** Rotation matrix of camera, in origin referential.
     * The format must be {U11, U12, U13, U21, U22, U23, U31, U32, U33},
     * where Uxy is the item of row x and column y. */
    boost::array<double, 9> rotationMatrix;
  };

  /// Struct with definitions of ARToolKitPlus targets.
  struct ArtkpTargets
  {
    /// ARToolKitPlus patter width [mm].
    int patternWidth;

    /// Each tuple is a pair of: the  path to the file describing one pattern
    /// in second position, and the unique identifier that silver will
    /// attribute to this pattern
    std::vector< boost::tuple<unsigned, std::string> > patterns;
  };

  typedef boost::variant<ArtkpTargets> VariantAnyTarget;

  struct Scene
  {
    boost::tuple< boost::optional<ArtkpTargets> > targets;

    std::vector<Camera> cameras;
  };
}
#endif /* _SCENE_HPP_ */
