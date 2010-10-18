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
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <string>
#include <vector>

namespace scene
{
  /// Base struct to all hardware camera configuration structs.
  /// This struct have all common configurations for all hardware cameras.
  struct Hardware
  {
    /// An unique identifier which differentiate this hardCamera from
    /// others hardCameras. This uniqueness must be system wide.
    unsigned hardCameraUid;

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

  struct PseudoCamera: public Hardware
  {
    /// Vector with path of all images.
    std::vector<std::string> imagesPath;

    /// The rate which the input images will be read.
    float frameRate;
  };

  struct DC1394: public Hardware
  {
    /// String representation of camera unique identifier in decimal base.
    std::string uid;

    /// The rate which the input images will be read.
    float frameRate;

    /// The color mode of the camera.
    /// One of: yuv411, yuyv, uyvy, rgb8, mono8 or mono16.
    std::string colorMode;

    boost::optional<std::string> bayerMethod;

    /// The order or color filter. One of rggb, gbrg, grbg or bggr.
    boost::optional<std::string> colorFilter;

    /// An array with [(blue/u), (red/v)].
    boost::optional<boost::array<std::string, 2> > whiteBalance;

    /// An array with [red, green, blue].
    boost::optional<boost::array<std::string, 3> > whiteShading;

    boost::optional<std::string> brightness;
    boost::optional<std::string> exposure;
    boost::optional<std::string> sharpness;
    boost::optional<std::string> hue;
    boost::optional<std::string> saturation;
    boost::optional<std::string> gamma;
    boost::optional<std::string> shutter;
    boost::optional<std::string> gain;
    boost::optional<std::string> iris;
    boost::optional<std::string> focus;
    boost::optional<std::string> temperature;
    boost::optional<std::string> trigger;
    boost::optional<std::string> triggerDelay;
    boost::optional<std::string> zoom;
    boost::optional<std::string> pan;
    boost::optional<std::string> tilt;
    boost::optional<std::string> opticalFilter;
    boost::optional<std::string> captureSize;
    boost::optional<std::string> captureQuality;
  };

  struct V4l2: public Hardware
  {
    /// This is the uid of v4l2 camera, it is given by order which they was
    /// connected in computer.
    unsigned uid;

    /// The color mode of camera. One of:
    /// rgb1, r444, rgbo, rgbp, rgbq, rgbr, bgr3, rgb3, bgr4, rgb4, y444, yuvo,
    /// yuvp, yuv4, grey, y16, yuyv, uyvy, y41p, yv12, yu12, yvu9, yuv9, 422p,
    /// 411p, nv12, nv21, mjpg.
    std::string colorMode;

    /// The descriptions of below controls was grabbed of
    /// http://v4l2spec.bytesex.org/spec/x542.htm

    boost::optional<int> brightness;
    boost::optional<int> contrast;   /// Contrast or luma gain.
    boost::optional<int> saturation; /// Color saturation or chroma gain.
    boost::optional<int> hue;        /// Hue or color balance.
    boost::optional<bool> autoWhiteBalance;
    boost::optional<int> redBalance;
    boost::optional<int> blueBalance;
    boost::optional<int> gamma;
    boost::optional<int> exposure;
    boost::optional<bool> autogain;  /// Automatic gain/exposure control.
    boost::optional<int> gain;
    boost::optional<bool> horizontalFlip; /// Mirror the picture horizontally.
    boost::optional<bool> verticalFlip; /// Mirror the picture vertically.
    /// Enables a power line frequency filter to avoid flicker.
    /// Possible values are: 0 (disabled), 50 [Hz] and 60 [Hz].
    boost::optional<int> powerLineFrequency;
    boost::optional<bool> hueAuto;
    /// This control specifies the white balance settings as a color
    /// temperature in Kelvin. A driver should have a minimum of 2800
    /// (incandescent) to 6500 (daylight).
    boost::optional<int> whiteBalanceTemperature;
    /// Adjusts the sharpness filters in a camera. The minimum value disables
    /// the filters, higher values give a sharper picture.
    boost::optional<int> sharpness;
    /// Adjusts the backlight compensation in a camera. The minimum value
    /// disables backlight compensation.
    boost::optional<int> backlightCompensation;
  };

  /// This is a boost::variant with all hardware camera models,
  /// i.e. all structs which inherit from Hardware struct.
  typedef boost::variant<PseudoCamera,
                         DC1394,
                         V4l2         > AnyHardwareCamera;

  /// Functor which receives a AnyHardwareCamera and return a copy
  /// of its Hardware base class.
  struct GetHardware :public boost::static_visitor<Hardware>
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
