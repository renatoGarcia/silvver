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

#ifndef _CF_PARSER_HPP_
#define _CF_PARSER_HPP_

#include <boost/array.hpp>
#include <stdexcept>
#include <string>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "scene.hpp"

class CfParser
{
public:

  class file_load_error : public std::logic_error
  {
  public:
    file_load_error(const std::string& whatArg)
      :logic_error(whatArg){};
  };

  CfParser();

  scene::Scene parseFile(const std::string& configFile);


private:

  scene::Scene sc;

  template <typename Type, int nItens>
  boost::array<Type, nItens> readArray(lua_State* L,
                                       const std::string& name) const;

  template <typename Type>
  Type readValue(lua_State* L, const std::string& name) const;

  // Return true if the field name exists
  bool hasField(lua_State* L, const std::string& name) const;

  void readCamera(lua_State* L);

  void  readHardware(lua_State* L, scene::Hardware& hardware);
  scene::PseudoCamera readPseudoCameraConfig(lua_State* L);
  scene::DC1394 readDC1394Config(lua_State* L);
  scene::V4l2 readV4l2Config(lua_State* L);

  void readTarget(lua_State* L);

  scene::ArtkpTargets readArtkpTargets(lua_State* L);
};


#endif // _CF_PARSER_HPP_
