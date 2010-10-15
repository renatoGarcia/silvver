/* Copyright 2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#include "cfReader.hpp"

#include <boost/bind.hpp>

#include "exceptions.hpp"
#include "luaParser.ipp"

CfReader::CfReader(const std::string& filePath)
  :luaParser(filePath)
{}

CfReader::~CfReader()
{}

scene::Scene
CfReader::readConfigFile()
{
  luaParser.loadGlobalTable("scene");

  luaParser.loadTable("cameras");
  luaParser.iterateArray(boost::bind(&CfReader::readCamera, this));
  luaParser.unloadTable();

  luaParser.loadTable("targets");
  luaParser.iterateArray(boost::bind(&CfReader::readTarget, this));
  luaParser.unloadTable();

  luaParser.unloadTable(); // Unload scene table

  return this->sc;
}

void
CfReader::readCamera()
{
  scene::Camera camera;

  std::string driver;
  luaParser.readValue(driver, "__driver");
  if (driver == "pseudocamera")
  {
    camera.hardware = this->readPseudoCameraConfig();
  }
  else if (driver == "dc1394")
  {
    camera.hardware = this->readDC1394Config();
  }
  else if (driver == "v4l2")
  {
    camera.hardware = this->readV4l2Config();
  }
  else
  {
    throw file_parsing_error()
      << info_what("Unknown driver name")
      << info_fieldName("__driver");
  }

  luaParser.readValue(camera.translationVector, "translation_vector");
  luaParser.readValue(camera.rotationMatrix, "rotation_matrix");

  this->sc.cameras.push_back(camera);
}

void
CfReader::readHardware(scene::Hardware& hardware)
{
  luaParser.readValue(hardware.hardCameraUid, "hardcamera_uid");
  luaParser.readValue(hardware.saveImageFormat, "save_image_format");
  luaParser.readValue(hardware.resolution, "resolution");

  luaParser.readValue(hardware.focalLength, "focal_length");
  luaParser.readValue(hardware.principalPoint, "principal_point");
  luaParser.readValue(hardware.radialCoef, "radial_coef");
  luaParser.readValue(hardware.tangentialCoef, "tangential_coef");
}

scene::PseudoCamera
CfReader::readPseudoCameraConfig()
{
  scene::PseudoCamera pseudoCamera;

  luaParser.readValue(pseudoCamera.imagesPath, "images_path");
  luaParser.readValue(pseudoCamera.frameRate, "frame_rate");
  this->readHardware(pseudoCamera);

  return pseudoCamera;
}

scene::DC1394
CfReader::readDC1394Config()
{
  scene::DC1394 dc1394;

  luaParser.readValue(dc1394.uid, "uid");
  luaParser.readValue(dc1394.frameRate, "frame_rate");
  luaParser.readValue(dc1394.colorMode, "color_mode");
  luaParser.readValue(dc1394.bayerMethod, "bayer_method");
  luaParser.readValue(dc1394.colorFilter, "color_filter");

  luaParser.readValue(dc1394.brightness, "brightness");
  luaParser.readValue(dc1394.exposure, "exposure");
  luaParser.readValue(dc1394.sharpness, "sharpness");
  luaParser.readValue(dc1394.hue, "hue");
  luaParser.readValue(dc1394.saturation, "saturation");
  luaParser.readValue(dc1394.gamma, "gamma");
  luaParser.readValue(dc1394.shutter, "shutter");
  luaParser.readValue(dc1394.gain, "gain");
  luaParser.readValue(dc1394.iris, "iris");
  luaParser.readValue(dc1394.focus, "focus");
  luaParser.readValue(dc1394.temperature, "temperature");
  luaParser.readValue(dc1394.trigger, "trigger");
  luaParser.readValue(dc1394.triggerDelay, "trigger_delay");
  luaParser.readValue(dc1394.zoom, "zoom");
  luaParser.readValue(dc1394.pan, "pan");
  luaParser.readValue(dc1394.tilt, "tilt");
  luaParser.readValue(dc1394.opticalFilter, "optical_filter");
  luaParser.readValue(dc1394.captureSize, "capture_size");
  luaParser.readValue(dc1394.captureQuality, "capture_quality");
  luaParser.readValue(dc1394.whiteBalance, "white_balance");
  luaParser.readValue(dc1394.whiteShading, "white_shading");

  this->readHardware(dc1394);

  return dc1394;
}

scene::V4l2
CfReader::readV4l2Config()
{
  scene::V4l2 v4l2;

  luaParser.readValue(v4l2.uid, "uid");
  luaParser.readValue(v4l2.colorMode, "color_mode");

  luaParser.readValue(v4l2.brightness, "brightness");
  luaParser.readValue(v4l2.contrast, "contrast");
  luaParser.readValue(v4l2.saturation, "saturation");
  luaParser.readValue(v4l2.hue, "hue");
  luaParser.readValue(v4l2.autoWhiteBalance, "auto_white_balance");
  luaParser.readValue(v4l2.redBalance, "red_balance");
  luaParser.readValue(v4l2.blueBalance, "blue_balance");
  luaParser.readValue(v4l2.gamma, "gamma");
  luaParser.readValue(v4l2.exposure, "exposure");
  luaParser.readValue(v4l2.autogain, "autogain");
  luaParser.readValue(v4l2.gain, "gain");
  luaParser.readValue(v4l2.horizontalFlip, "horizontal_flip");
  luaParser.readValue(v4l2.verticalFlip, "vertical_flip");
  luaParser.readValue(v4l2.powerLineFrequency, "power_line_frequency");
  luaParser.readValue(v4l2.hueAuto, "hue_auto");
  luaParser.readValue(v4l2.whiteBalanceTemperature, "white_balance_temperature");
  luaParser.readValue(v4l2.sharpness, "sharpness");
  luaParser.readValue(v4l2.backlightCompensation, "backlight_compensation");

  this->readHardware(v4l2);

  return v4l2;
}

void
CfReader::readTarget()
{
  scene::AnyTarget target;

  std::string type;
  luaParser.readValue(type, "__type");
  if (type == "artkp")
  {
    target = this->readArtkp();
  }
  else if (type == "butterfly")
  {
    target = this->readButterfly();
  }
  else
  {
    throw file_parsing_error()
      << info_what("Unknown target system name")
      << info_fieldName("__type");
  }

  this->sc.targets.push_back(target);
}

scene::ArtkpTargets
CfReader::readArtkp()
{
  boost::shared_ptr<scene::ArtkpTargets> artkpTargets(new scene::ArtkpTargets());

  luaParser.readValue(artkpTargets->patternWidth, "pattern_width");
  luaParser.readValue(artkpTargets->threshold, "threshold");
  luaParser.readValue(artkpTargets->bodyTranslation, "body_translation");
  luaParser.readValue(artkpTargets->bodyRotation, "body_rotation");
  luaParser.readValue(artkpTargets->silvverUid, "silvver_uid");

  luaParser.iterateArray(boost::bind(&CfReader::readArtkpTargets, this,
                                     artkpTargets));

  return *artkpTargets;
}

void
CfReader::readArtkpTargets(boost::shared_ptr<scene::ArtkpTargets> artkpTargets)
{
  std::string patternPath;
  unsigned uid;

  luaParser.readValue(patternPath, "pattern_file");
  luaParser.readValue(uid, "uid");

  artkpTargets->patterns.push_back(boost::make_tuple(uid, patternPath));
}

scene::ButterflyTargets
CfReader::readButterfly()
{
  scene::ButterflyTargets butterflyTargets;
  luaParser.readValue(butterflyTargets.squareSize, "square_size");
  luaParser.readValue(butterflyTargets.maxButterflies, "max_butterflies");
  luaParser.readValue(butterflyTargets.bodyTranslation, "body_translation");
  luaParser.readValue(butterflyTargets.bodyRotation, "body_rotation");
  luaParser.readValue(butterflyTargets.silvverUid, "silvver_uid");

  return butterflyTargets;
}
