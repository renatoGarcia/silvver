#include <iostream>
using namespace std;

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

  lua_getfield(L, -1, "hardware");
  if (lua_isnil(L, -1))
  {
    throw file_load_error("An instance of field <hardware> "\
                          "was don't found in config file");
  }
  if (lua_istable(L, -1)) // hardcamera is pseudocamera
  {
    camera.imagesPath = readValue<std::string>(L, "path");
    lua_pop(L,1); // pop hardware table
    camera.hardware = readArray<std::string, 1>(L, "hardware").at(0);
  }
  else
  {
    lua_pop(L,1); // pop hardware field
    camera.hardware = readValue<std::string>(L, "hardware");
  }

  camera.uid = readValue<std::string>(L, "uid");
  camera.resolution = readArray<unsigned, 2>(L, "resolution");
  camera.frameRate = readValue<float>(L, "frame_rate");

  camera.focalLength = readArray<double, 2>(L, "focal_length");
  camera.principalPoint = readArray<double, 2>(L, "principal_point");
  camera.radialCoef = readArray<double, 3>(L, "radial_coef");
  camera.tangentialCoef = readArray<double, 2>(L, "tangential_coef");

  camera.translationVector = readArray<double, 3>(L, "translation_vector");
  camera.rotationMatrix = readArray<double, 9>(L, "rotation_matrix");

  this->sc.cameras.push_back(camera);
}

void
CfParser::readArtkpTargets(lua_State* L)
{
  scene::ArtkpTargets artkpTargets;

  // artkpTargets.patternWidth = readValue<int>(L, "pattern_width");

  std::string patternPath;
  unsigned uid;
  lua_pushnil(L);// first key
  while (lua_next(L, -2) != 0)
  {
    patternPath = readValue<std::string>(L, "pattern_file");
    uid = readValue<unsigned>(L, "uid");

    artkpTargets.patterns.push_back(boost::make_tuple(uid, patternPath));

    // removes 'value', keeps 'key' for next iteration
    lua_pop(L, 1);
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

  lua_getfield(L, -1, "camera");
  if (!lua_istable(L, -1))
  {
    throw file_load_error("Don't found camera variable in config file");
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
