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

#ifndef _DC1394_2X_HPP_
#define _DC1394_2X_HPP_

#include "hardCamera.hpp"

#include <boost/array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <string>

#include <dc1394/dc1394.h>

class DC1394: public HardCamera
{
public:

  DC1394(const scene::DC1394& config);

  ~DC1394();

private:

  static const int N_BUFFERS = 4;

  void doWork();

  // Convert the HardCamera frameWidth and frameHeight attributes to an
  // equivalent dc1394video_mode_t
  dc1394video_mode_t getDc1394VideoMode(const std::string& colorMode) const;

  // Convert the HardCamera frameRate to an equivalent dc1394framerate_t
  dc1394framerate_t getDc1394FrameRate(const float frameRate) const;

  std::pair<int,int> getPairDepthChannels(const std::string& colorMode) const;

  int getIplDepth(const std::string& colorMode) const;

  int getNChannels(const std::string& colorMode) const;

  /// Set all features of this camera.
  /// @param config The configurations of this camera.
  void setFeatures(const scene::DC1394& config);

  /// Set the power mode of a feature.
  /// This method is called by setFeatures method.
  /// @param featureInfo The information about the feature.
  /// @param strValue The value to set in string format.
  /// @param featureName A string representation of feature.
  /// @return True if set on, false else.
  bool setPower(dc1394feature_info_t featureInfo,
                std::string strValue, std::string featureName);

  /// Set the auto mode of a feature.
  /// This method is called by setFeatures method.
  /// @param featureInfo The information about the feature.
  /// @param strValue The value to set in string format.
  /// @param featureName A string representation of feature.
  /// @return True if set auto, false else.
  bool setAutoMode(dc1394feature_info_t featureInfo,
                   std::string strValue, std::string featureName);

  void setFeatureValue(dc1394feature_info_t featureInfo,
                       std::string strValue, std::string featureName);

  void setWhiteBalance(dc1394feature_info_t featureInfo,
                       std::string blueU, std::string redV);

  void setWhiteShading(dc1394feature_info_t featureInfo,
                       std::string red, std::string green, std::string blue);

  /// A string representing the uid of the camera.
  /// It must be numbers in decimal base.
  const std::string uid;

  const dc1394video_mode_t videoMode;

  dc1394_t* context;

  dc1394camera_t* camera;

  boost::scoped_ptr<boost::thread> grabFrameThread;
};

#endif /* _DC1394_2X_HPP_ */
