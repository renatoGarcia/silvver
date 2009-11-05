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

#include <map>
#include <string>
#include <vector>

#include <boost/array.hpp>
#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/variant.hpp>

namespace scene
{

  /** Base struct to all hardware camera configuration structs.
   * This struct have all common configurations for all hardware cameras.
   */
  struct Hardware
  {
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
  };

  struct PseudoCamera : public Hardware
  {
    /// Path to directory where are the input images.
    std::string imagesPath;

    /// The rate which the input images will be read.
    float frameRate;
  };

  struct DC1394 : public Hardware
  {
    /// The rate which the input images will be read.
    float frameRate;

    /** The color mode of the camera.
     * One of: yuv444, yuv422, yuv411, rgb8, mono8 or mono16. All letters must
     * be in lower case.
     */
    std::string colorMode;

    /** The brightness control.
     * One of "untouched" or an unsigned integer between 0 and 255.
     */
    std::string brightness;

    /** The auto exposure control.
     * One of "untouched", "off", "auto" or an unsigned integer between
     * 1 and 1023.
     */
    std::string exposure;

    /** The white balance control.
     * An array with [(blue/v), (red/v)].
     * Each position must be one of (the same in both) "untouched", "off" or
     * an unsigned integer between 0 and 63.
     */
    boost::array<std::string, 2> whiteBalance;

    /** The shuter timer control.
     * One of "untouched", "auto" or an unsigned integer greater than  2.
     */
    std::string shutter;

     /** The gain control.
     * One of "untouched", "auto" or an unsigned integer between 220 and 1023.
     */
    std::string gain;
  };

  /// This is a boost::variant with all hardware camera models, i.e. all structs which inherit from Hardware struct.
  typedef boost::variant<PseudoCamera, DC1394> VariantHardwareCamera;

  /// Functor which receives a VariantHardwareCamera and return a copy of its Hardware base class.
  struct GetHardware : public boost::static_visitor<Hardware>
  {
    template <class T>
    const Hardware operator()(const T& config) const
    {
      return config;
    }

    template <class T>
    Hardware operator()(T& config)
    {
      return config;
    }
  };

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

  /// boost::variant with all structs of target configurations.
  typedef boost::variant<ArtkpTargets> VariantAnyTarget;

  /// Complete description of scene, with cameras and targets.
  struct Scene
  {
    /// Tuple with configuration structs of all targets in scene, and only these.
    boost::tuple< boost::optional<ArtkpTargets> > targets;

    /// Vector with configuration structs of all camera in scene.
    std::vector<Camera> cameras;
  };
}
#endif /* _SCENE_HPP_ */
