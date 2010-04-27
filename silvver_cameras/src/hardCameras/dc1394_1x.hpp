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

#ifndef _DC1394_1X_HPP_
#define _DC1394_1X_HPP_

#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <string>
#include <utility>

#include <libdc1394/dc1394_control.h>
#include <libraw1394/raw1394.h>

#include "colorConverter.hpp"
#include "hardCamera.hpp"

class DC1394: public HardCamera
{
public:

  DC1394(const scene::DC1394& config);

  ~DC1394();

private:

  static const int N_BUFFERS = 4;

  void doWork();

  void findThisCamera(nodeid_t& node, int& cardIndex);

  std::string findVideo1394Device(unsigned cardNumber);

  // Convert a float frameRate to an equivalent DC1394 frame rate
  static int getDc1394FrameRate(float frameRate);

  static unsigned getDc1394VideoMode(const scene::DC1394& config);

  static int getIplDepth(const std::string& colorMode);

  static ColorConverter createColorConverter(const scene::DC1394& config);

  /// Set all features of this camera.
  /// @param config The configurations of this camera.
  /// @param cameraNode The node of this camera.
  void setFeatures(const scene::DC1394& config, nodeid_t cameraNode);

  /// Set the on/off mode of a feature.
  /// This method is called by setFeatures method.
  /// @param cameraNode The node of this camera.
  /// @param featureInfo The information about the feature.
  /// @param strValue The value to set in string format.
  /// @param featureName A string representation of feature.
  /// @return True if set on, false else.
  bool setOnOffFeature(nodeid_t cameraNode, dc1394_feature_info featureInfo,
                       std::string strValue, std::string featureName);

  /// Set the auto mode of a feature.
  /// This method is called by setFeatures method.
  /// @param cameraNode The node of this camera.
  /// @param featureInfo The information about the feature.
  /// @param strValue The value to set in string format.
  /// @param featureName A string representation of feature.
  /// @return True if set auto, false else.
  bool setAutoFeature(nodeid_t cameraNode, dc1394_feature_info featureInfo,
                      std::string strValue, std::string featureName);

  void setFeatureValue(nodeid_t cameraNode, dc1394_feature_info featureInfo,
                       std::string strValue, std::string featureName);

  void setWhiteBalance(nodeid_t cameraNode, dc1394_feature_info featureInfo,
                       std::string blueU, std::string redV);

  void setWhiteShading(nodeid_t cameraNode, dc1394_feature_info featureInfo,
                       std::string red, std::string green, std::string blue);

  /// A string representing the uid of the camera.
  /// It must be numbers in decimal base.
  const std::string uid;

  raw1394handle_t raw1394Handle;

  dc1394_cameracapture dc1394Camera;
  bool                 bDc1394CameraCreated;

  const int videoMode;

  ColorConverter colorConverter;

  boost::scoped_ptr<boost::thread> grabFrameThread;
};

#endif /* _DC1394_1X_HPP_ */
