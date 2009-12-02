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

/** \file cameraDescriptions.hpp
 * \brief This file contain all structs with the needed configuration of each
 * hardCamera model. The data described in this file will be read from the lua
 * config file, and passed to related camera class to proper initialization.
 */

#ifndef _CAMERA_DESCRIPTIONS_HPP_
#define _CAMERA_DESCRIPTIONS_HPP_

#include <boost/array.hpp>
#include <boost/variant.hpp>
#include <string>

namespace scene
{
  /** Base struct to all hardware camera configuration structs.
   * This struct have all common configurations for all hardware cameras.
   */
  struct Hardware
  {
    /// An unique string which differentiate this hardCamera from others hardCamera.
    std::string identifier;

    /// String configuring the name of saved images. This string can have
    /// the placeholders %1%, %2% and %3%; where %1% is the camera identifier,
    /// %2% is the saved images counter, and the %3% is the string "d" or
    /// "u" indicating if the saving image was undistorted or not.
    std::string saveImageFormat;

    /// {width, height}.
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
    /// String representation of camera unique identifier in decimal base.
    std::string uid;

    /// The rate which the input images will be read.
    float frameRate;

    /** The color mode of the camera.
     * One of: yuv444, yuv422, yuv411, rgb8, mono8 or mono16. All letters must
     * be in lower case.
     */
    std::string colorMode;

    /** The brightness control.
     * One of: "untouched" or an unsigned integer between 0 and 255.
     */
    std::string brightness;

    /** The auto exposure control.
     * One of: "untouched", "off", "auto" or an unsigned integer between
     * 1 and 1023.
     */
    std::string exposure;

    /** The white balance control.
     * An array with [(blue/v), (red/v)].
     * Each position must be one of (the same in both): "untouched", "off" or
     * an unsigned integer between 0 and 63.
     */
    boost::array<std::string, 2> whiteBalance;

    /** The shuter timer control.
     * One of: "untouched", "auto" or an unsigned integer greater than  2.
     */
    std::string shutter;

    /** The gain control.
     * One of: "untouched", "auto" or an unsigned integer
     * between 220 and 1023.
     */
    std::string gain;
  };

  /** This is a boost::variant with all hardware camera models,
   * i.e. all structs which inherit from Hardware struct.
   */
  typedef boost::variant<PseudoCamera, DC1394> AnyHardwareCamera;

  /** Functor which receives a AnyHardwareCamera and return a copy
   * of its Hardware base class.
   */
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

}

#endif /* _CAMERA_DESCRIPTIONS_HPP_ */
