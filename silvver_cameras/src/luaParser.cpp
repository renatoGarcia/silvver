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

#include "luaParser.hpp"
#include "luaParser.ipp"

#include <boost/lexical_cast.hpp>

#include "exceptions.hpp"

LuaParser::LuaParser(const std::string& luaFile)
  :L(lua_open())
{
  luaL_openlibs(this->L);

  if (luaL_dofile(this->L, luaFile.c_str()))
  {
    throw load_file_error()
      << info_what("Error on loading lua config file, " +
                   std::string(lua_tostring(this->L, -1)));
  }
}

LuaParser::~LuaParser()
{
  if (this->L)
    lua_close(L);
}

void
LuaParser::loadGlobalTable(const std::string& name)
{
  lua_getglobal(this->L, name.c_str());
  if (!lua_istable(this->L, -1))
  {
    throw type_error()
      << info_what("Global is not a table")
      << info_fieldName(name);
  }
}

void
LuaParser::loadTable(const std::string& name)
{
  lua_getfield(this->L, -1, name.c_str());
}

void
LuaParser::unloadTable()
{
  lua_pop(this->L, 1);
}

void
LuaParser::iterateArray(boost::function<void (void)> callback)
{
  lua_pushnil(this->L);// first key
  while (lua_next(this->L, -2) != 0)
  {
    try
    {
      callback();
    }
    catch (file_parsing_error& e)
    {
      throw e << info_cameraIndex(lua_tonumber(this->L, -2));
    }

    // removes 'value', keeps 'key' for next iteration
    lua_pop(this->L, 1);
  }
}

bool
LuaParser::hasField(const std::string& name) const
{
  lua_getfield(this->L, -1, name.c_str());

  if (lua_isnil(this->L, -1))
  {
    lua_pop(this->L, 1);
    return false;
  }
  else
  {
    lua_pop(this->L, 1);
    return true;
  }
}

void
LuaParser::getTop(double& output) const
{
  if (lua_isnil(this->L, -1))
  {
    throw missing_field() << info_what("Missing field");
  }
  else if (!lua_isnumber(this->L, -1))
  {
    throw type_error() << info_what("Field is not a number");
  }
  else
  {
    output = lua_tonumber(this->L, -1);
  }
}

void
LuaParser::getTop(float& output) const
{
  double d;
  getTop(d);
  output = float(d);
}

void
LuaParser::getTop(int& output) const
{
  double d;
  getTop(d);
  output = (int)d;
}

void
LuaParser::getTop(unsigned int& output) const
{
  double d;
  getTop(d);
  output = unsigned(d);
}

void
LuaParser::getTop(bool& output) const
{
  if (lua_isnil(this->L, -1))
  {
    throw missing_field() << info_what("Missing field");
  }
  else if (!lua_isboolean(this->L, -1))
  {
    throw type_error() << info_what("Field is not a boolean");
  }
  else
  {
    output = lua_toboolean(this->L, -1);
  }
}

void
LuaParser::getTop(std::string& output) const
{
  if (lua_isnil(this->L, -1))
  {
    throw missing_field() << info_what("Missing field");
  }
  else if (!lua_isstring(this->L, -1))
  {
    throw type_error() << info_what("Field is not a string");
  }
  else
  {
    output = lua_tostring(this->L, -1);
  }
}
