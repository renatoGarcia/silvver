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

#include "cfParser.hpp"

#include <boost/lexical_cast.hpp>

CfParser::CfParser()
{}

template <typename Type, int nItens>
boost::array<Type, nItens>
CfParser::readArray(lua_State* L, std::string name)
{
  lua_getfield(L, -1, name.c_str());

  if (lua_isnil(L, -1))
  {
    throw file_load_error("An instance of array <" + name +
                          "> was don't found in config file");
  }
  if (!lua_istable(L, -1))
  {
    throw file_load_error("Field name <" + name + "> isn't an array");
  }

  boost::array<Type, nItens> returnArray;

  // Put all items on stack
  for (int i = 1; i <= nItens; ++i)
  {
    lua_pushnumber(L, i);
    lua_gettable(L, -(i + 1));

    if (lua_isnil(L, -1))
    {
      throw file_load_error("Index " +
                            boost::lexical_cast<std::string>(i) +
                            " from <" + name + "> array don't exists");
    }
    if (!lua_isstring(L, -1))
    {
      throw file_load_error("The index " +
                            boost::lexical_cast<std::string>(i) +
                            " from <" + name +
                            "> array isn't a number or string");
    }
  }

  // Convert all items on stack and put on boost::array
  for (int i = -nItens; i < 0; ++i)
  {
    try
    {
      returnArray.at(nItens + i) =
        boost::lexical_cast<Type>(lua_tostring(L, i));
    }
    catch (boost::bad_lexical_cast &)
    {
      throw file_load_error("The index " +
                            boost::lexical_cast<std::string>(nItens+i+1) +
                            " from <" + name + "> array " +
                            "couldn't be converted to appropriated type");
    }
  }

  lua_pop(L, nItens + 1);

  return returnArray;
}

template <typename Type>
Type
CfParser::readValue(lua_State* L, std::string name)
{
  lua_getfield(L, -1, name.c_str());

  if (lua_isnil(L, -1))
  {
    throw file_load_error("An instance of field <" + name +
                          "> was don't found in config file");
  }
  if (!lua_isstring(L, -1))
  {
    throw file_load_error("Field name <" + name +
                          "> isn't a number or string");
  }

  Type returnValue;
  try
  {
    returnValue = boost::lexical_cast<Type>(lua_tostring(L, -1));
  }
  catch (boost::bad_lexical_cast &)
  {
    throw file_load_error("The field <" + name +
                          "> couldn't be converted to the appropriated type");
  }

  lua_pop(L,1);

  return returnValue;
}

bool
CfParser::hasField(lua_State* L, std::string name)
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

  std::string name = readValue<std::string>(L, "__name");
  if (name == "pseudocamera")
  {
    camera.hardware = this->readPseudoCameraConfig(L);
  }
  else if (name == "dc1394")
  {
    camera.hardware = this->readDC1394Config(L);
  }
  else
  {
    throw file_load_error("Unknown hardware camera name");
  }

  camera.translationVector = readArray<double, 3>(L, "translation_vector");
  camera.rotationMatrix = readArray<double, 9>(L, "rotation_matrix");

  this->sc.cameras.push_back(camera);
}

void
CfParser::readHardware(lua_State* L, scene::Hardware& hardware)
{
  hardware.uid = readValue<std::string>(L, "uid");
  hardware.resolution = readArray<unsigned, 2>(L, "resolution");

  hardware.focalLength = readArray<double, 2>(L, "focal_length");
  hardware.principalPoint = readArray<double, 2>(L, "principal_point");
  hardware.radialCoef = readArray<double, 3>(L, "radial_coef");
  hardware.tangentialCoef = readArray<double, 2>(L, "tangential_coef");
}

scene::PseudoCamera
CfParser::readPseudoCameraConfig(lua_State* L)
{
  scene::PseudoCamera pseudoCamera;

  this->readHardware(L, pseudoCamera);
  pseudoCamera.imagesPath = readValue<std::string>(L, "images_path");
  pseudoCamera.frameRate = readValue<float>(L, "frame_rate");

  return pseudoCamera;
}

scene::DC1394
CfParser::readDC1394Config(lua_State* L)
{
  scene::DC1394 dc1394;

  this->readHardware(L, dc1394);
  dc1394.frameRate    = readValue<float>(L, "frame_rate");
  dc1394.colorMode    = readValue<std::string>(L, "color_mode");
  dc1394.brightness   = readValue<std::string>(L, "brightness");
  dc1394.exposure     = readValue<std::string>(L, "exposure");
  dc1394.whiteBalance = readArray<std::string, 2>(L, "white_balance");
  dc1394.shutter      = readValue<std::string>(L, "shutter");
  dc1394.gain         = readValue<std::string>(L, "gain");

  return dc1394;
}

void
CfParser::readArtkpTargets(lua_State* L)
{
  scene::ArtkpTargets artkpTargets;

  artkpTargets.patternWidth = readValue<int>(L, "pattern_width");

  std::string patternPath;
  unsigned uid;

  int nTargets = lua_objlen(L, -1);
  for(int i = 1; i <= nTargets; ++i)
  {
    lua_pushnumber(L, i);
    lua_gettable(L, -2); // Get the i artkp target

    patternPath = readValue<std::string>(L, "pattern_file");
    uid = readValue<unsigned>(L, "uid");

    artkpTargets.patterns.push_back(boost::make_tuple(uid, patternPath));

    lua_pop(L, 1); // pop the i artkp target
  }

  boost::get<0>(this->sc.targets).reset(artkpTargets);
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
    throw file_load_error("Don't found scene variable in config file");
  }

  lua_getfield(L, -1, "cameras");
  if (!lua_istable(L, -1))
  {
    throw file_load_error("Don't found cameras variable in config file");
  }

  lua_pushnil(L);// first key
  while (lua_next(L, -2) != 0)
  {
    readCamera(L);

    // removes 'value', keeps 'key' for next iteration
    lua_pop(L, 1);
  }
  lua_pop(L, 1); // pop camera field

  if (hasField(L, "artp_targets"))
  {
    lua_getfield(L, -1, "artp_targets");
    readArtkpTargets(L);
    lua_pop(L, 1); // pop artp_targets field
  }

  return this->sc;
}
