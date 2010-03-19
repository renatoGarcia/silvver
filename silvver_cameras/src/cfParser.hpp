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
#include <boost/optional.hpp>
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
  CfParser();

  scene::Scene parseFile(const std::string& configFile);

private:
  scene::Scene sc;

  void getTop(double& output, lua_State* L) const;

  void getTop(float& output, lua_State* L) const;

  void getTop(int& output, lua_State* L) const;

  void getTop(unsigned int& output, lua_State* L) const;

  void getTop(bool& output, lua_State* L) const;

  void getTop(std::string& output, lua_State* L) const;

  template <class Type, std::size_t nItens>
  void getTop(boost::array<Type, nItens>& output, lua_State* L) const;

  template<class Type>
  void readValue(Type& output,
                 lua_State* L, const std::string& fieldName) const;

  template<class Type>
  void readValue(boost::optional<Type>& output,
                 lua_State* L, const std::string& fieldName) const;

  // Return true if the field name exists
  bool hasField(lua_State* L, const std::string& fieldName) const;

  void readCamera(lua_State* L);

  void  readHardware(lua_State* L, scene::Hardware& hardware);
  scene::PseudoCamera readPseudoCameraConfig(lua_State* L);
  scene::DC1394 readDC1394Config(lua_State* L);
  scene::V4l2 readV4l2Config(lua_State* L);

  void readTarget(lua_State* L);

  scene::ArtkpTargets readArtkpTargets(lua_State* L);
};


#endif // _CF_PARSER_HPP_
