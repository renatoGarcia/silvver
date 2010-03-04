/* Copyright 2009,2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#include "cfParser.hpp"

#include <boost/lexical_cast.hpp>

CfParser::CfParser()
{}

void
CfParser::getTop(double& output, lua_State* L) const
{
  if (lua_isnil(L, -1))
  {
    throw missing_field("");
  }
  else if (!lua_isnumber(L, -1))
  {
    throw field_read_error("is not a number");
  }
  else
  {
    output = lua_tonumber(L, -1);
  }
}

void
CfParser::getTop(float& output, lua_State* L) const
{
  double d;
  getTop(d, L);
  output = float(d);
}

void
CfParser::getTop(int& output, lua_State* L) const
{
  double d;
  getTop(d, L);
  output = (int)d;
}

void
CfParser::getTop(unsigned int& output, lua_State* L) const
{
  double d;
  getTop(d, L);
  output = unsigned(d);
}

void
CfParser::getTop(bool& output, lua_State* L) const
{
  if (lua_isnil(L, -1))
  {
    throw missing_field("");
  }
  else if (!lua_isboolean(L, -1))
  {
    throw field_read_error("is not a boolean");
  }
  else
  {
    output = lua_toboolean(L, -1);
  }
}

void
CfParser::getTop(std::string& output, lua_State* L) const
{
  if (lua_isnil(L, -1))
  {
    throw missing_field("");
  }
  else if (!lua_isstring(L, -1))
  {
    throw field_read_error("is not a string");
  }
  else
  {
    output = lua_tostring(L, -1);
  }
}

template <class Type, std::size_t nItens>
void
CfParser::getTop(boost::array<Type, nItens>& output, lua_State* L) const
{
  if (lua_isnil(L, -1))
  {
    throw missing_field("");
  }
  if (!lua_istable(L, -1))
  {
    throw field_read_error("is not an array");
  }

  for (int i = 1; i <= nItens; ++i)
  {
    lua_pushnumber(L, i);
    lua_gettable(L, -2);

    try
    {
      getTop(output.at(i-1), L);
      lua_pop(L, 1); // Pop the current item
    }
    catch (missing_field& e)
    {
      lua_pop(L, 1); // Pop the current nil item

      // The table is not nil, thus throws a field_read_error
      throw field_read_error("has not the array index " +
                             boost::lexical_cast<std::string>(i));
    }
    catch (field_read_error& e)
    {
      lua_pop(L,1); // Pop the current wrong item
      throw field_read_error(",index " + boost::lexical_cast<std::string>(i) +
                             " " + e.what());
    }
  }
}

template<class Type>
void
CfParser::readValue(Type& output,
                    lua_State* L, const std::string& fieldName) const
{
  lua_getfield(L, -1, fieldName.c_str());
  try
  {
    getTop(output, L);
    lua_pop(L, 1);
  }
  catch(missing_field& e)
  {
    lua_pop(L, 1);
    throw missing_field("An instance of field <" + fieldName +
                        "> was not found in config file");
  }
  catch(field_read_error& e)
  {
    lua_pop(L, 1);
    throw field_read_error("The field <" + fieldName + "> " + e.what());
  }
}

template <class Type>
void
CfParser::readValue(boost::optional<Type>& output,
                    lua_State* L, const std::string& fieldName) const
{
  lua_getfield(L, -1, fieldName.c_str());
  try
  {
    Type tmp;
    getTop(tmp, L);
    output = tmp;
    lua_pop(L, 1);
  }
  catch(missing_field& e)
  {
    output.reset();
    lua_pop(L, 1);
  }
  catch(field_read_error& e)
  {
    lua_pop(L, 1);
    throw field_read_error("The field <" + fieldName + "> " + e.what());
  }
}

bool
CfParser::hasField(lua_State* L, const std::string& name) const
{
  lua_getfield(L, -1, name.c_str());

  if (lua_isnil(L, -1))
  {
    lua_pop(L, 1);
    return false;
  }
  else
  {
    lua_pop(L, 1);
    return true;
  }
}

void
CfParser::readCamera(lua_State* L)
{
  scene::Camera camera;

  std::string driver;
  readValue(driver, L, "__driver");
  if (driver == "pseudocamera")
  {
    camera.hardware = this->readPseudoCameraConfig(L);
  }
  else if (driver == "dc1394")
  {
    camera.hardware = this->readDC1394Config(L);
  }
  else if (driver == "v4l2")
  {
    camera.hardware = this->readV4l2Config(L);
  }
  else
  {
    throw file_parsing_error("Unknown hardware camera name");
  }

  readValue(camera.translationVector, L, "translation_vector");
  readValue(camera.rotationMatrix, L, "rotation_matrix");

  this->sc.cameras.push_back(camera);
}

void
CfParser::readHardware(lua_State* L, scene::Hardware& hardware)
{
  readValue(hardware.identifier, L, "identifier");
  readValue(hardware.saveImageFormat, L, "save_image_format");
  readValue(hardware.resolution, L, "resolution");

  readValue(hardware.focalLength, L, "focal_length");
  readValue(hardware.principalPoint, L, "principal_point");
  readValue(hardware.radialCoef, L, "radial_coef");
  readValue(hardware.tangentialCoef, L, "tangential_coef");
}

scene::PseudoCamera
CfParser::readPseudoCameraConfig(lua_State* L)
{
  scene::PseudoCamera pseudoCamera;

  readValue(pseudoCamera.imagesPath, L, "images_path");
  readValue(pseudoCamera.frameRate, L, "frame_rate");
  this->readHardware(L, pseudoCamera);

  return pseudoCamera;
}

scene::DC1394
CfParser::readDC1394Config(lua_State* L)
{
  scene::DC1394 dc1394;

  readValue(dc1394.uid, L, "uid");
  readValue(dc1394.frameRate, L, "frame_rate");
  readValue(dc1394.colorMode, L, "color_mode");
  readValue(dc1394.bayerMethod, L, "bayer_method");

  readValue(dc1394.brightness, L, "brightness");
  readValue(dc1394.exposure, L, "exposure");
  readValue(dc1394.sharpness, L, "sharpness");
  readValue(dc1394.hue, L, "hue");
  readValue(dc1394.saturation, L, "saturation");
  readValue(dc1394.gamma, L, "gamma");
  readValue(dc1394.shutter, L, "shutter");
  readValue(dc1394.gain, L, "gain");
  readValue(dc1394.iris, L, "iris");
  readValue(dc1394.focus, L, "focus");
  readValue(dc1394.temperature, L, "temperature");
  readValue(dc1394.trigger, L, "trigger");
  readValue(dc1394.triggerDelay, L, "trigger_delay");
  readValue(dc1394.zoom, L, "zoom");
  readValue(dc1394.pan, L, "pan");
  readValue(dc1394.tilt, L, "tilt");
  readValue(dc1394.opticalFilter, L, "optical_filter");
  readValue(dc1394.captureSize, L, "capture_size");
  readValue(dc1394.captureQuality, L, "capture_quality");
  readValue(dc1394.whiteBalance, L, "white_balance");
  readValue(dc1394.whiteShading, L, "white_shading");

  this->readHardware(L, dc1394);

  return dc1394;
}

scene::V4l2
CfParser::readV4l2Config(lua_State* L)
{
  scene::V4l2 v4l2;

  readValue(v4l2.uid, L, "identifier");
  this->readHardware(L, v4l2);

  readValue(v4l2.brightness, L, "brightness");
  readValue(v4l2.contrast, L, "contrast");
  readValue(v4l2.saturation, L, "saturation");
  readValue(v4l2.hue, L, "hue");
  readValue(v4l2.autoWhiteBalance, L, "auto_white_balance");
  readValue(v4l2.redBalance, L, "red_balance");
  readValue(v4l2.blueBalance, L, "blue_balance");
  readValue(v4l2.gamma, L, "gamma");
  readValue(v4l2.exposure, L, "exposure");
  readValue(v4l2.autogain, L, "autogain");
  readValue(v4l2.gain, L, "gain");
  readValue(v4l2.horizontalFlip, L, "horizontal_flip");
  readValue(v4l2.verticalFlip, L, "vertical_flip");
  readValue(v4l2.powerLineFrequency, L, "power_line_frequency");
  readValue(v4l2.hueAuto, L, "hue_auto");
  readValue(v4l2.whiteBalanceTemperature, L, "white_balance_temperature");
  readValue(v4l2.sharpness, L, "sharpness");
  readValue(v4l2.backlightCompensation, L, "backlight_compensation");

  return v4l2;
}

void
CfParser::readTarget(lua_State* L)
{
  scene::AnyTarget target;

  std::string type;
  readValue(type, L, "__type");
  if (type == "artkp")
  {
    target = this->readArtkpTargets(L);
  }
  else
  {
    throw file_parsing_error("Unknown target system name");
  }

  this->sc.targets.push_back(target);
}

scene::ArtkpTargets
CfParser::readArtkpTargets(lua_State* L)
{
  // Each artkp config struct must have a unique key. What is the value is
  // indifferent, but it must be unique.
  static unsigned uniqueKey = 0;
  uniqueKey++;

  scene::ArtkpTargets artkpTargets;

  artkpTargets.uniqueKey = uniqueKey;
  readValue(artkpTargets.patternWidth, L, "pattern_width");
  readValue(artkpTargets.threshold, L, "threshold");
  readValue(artkpTargets.bodyTranslation, L, "body_translation");
  readValue(artkpTargets.bodyRotation, L, "body_rotation");

  std::string patternPath;
  unsigned uid;

  int nTargets = lua_objlen(L, -1);
  for(int i = 1; i <= nTargets; ++i)
  {
    lua_pushnumber(L, i);
    lua_gettable(L, -2); // Get the i artkp target

    readValue(patternPath, L, "pattern_file");
    readValue(uid, L, "uid");

    artkpTargets.patterns.push_back(boost::make_tuple(uid, patternPath));

    lua_pop(L, 1); // pop the i artkp target
  }

  return artkpTargets;
}

scene::Scene
CfParser::parseFile(const std::string& configFile)
{
  lua_State* L = lua_open();
  luaL_openlibs(L);

  if (luaL_dofile(L, configFile.c_str()))
  {
    throw std::invalid_argument("Error on loading config file, " +
                                std::string(lua_tostring(L, -1)));
  }

  lua_getglobal(L, "scene");
  if (!lua_istable(L, -1))
  {
    throw file_parsing_error("Don't found scene variable in config file");
  }

  //------------------------------------ Cameras
  lua_getfield(L, -1, "cameras");
  if (!lua_istable(L, -1))
  {
    throw file_parsing_error("Don't found cameras table in config file");
  }

  lua_pushnil(L);// first key
  while (lua_next(L, -2) != 0)
  {
    readCamera(L);

    // removes 'value', keeps 'key' for next iteration
    lua_pop(L, 1);
  }
  lua_pop(L, 1); // pop camera field

  //------------------------------------ Targets
  lua_getfield(L, -1, "targets");
  if (!lua_istable(L, -1))
  {
    throw file_parsing_error("Don't found targets table in config file");
  }

  lua_pushnil(L);// first key
  while (lua_next(L, -2) != 0)
  {
    readTarget(L);

    // removes 'value', keeps 'key' for next iteration
    lua_pop(L, 1);
  }
  lua_pop(L, 1); // pop targets field

  //----------------------------------------------

  return this->sc;
}
