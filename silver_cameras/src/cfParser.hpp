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

#ifndef CF_PARSER_HPP
#define CF_PARSER_HPP

#include <stdexcept>
#include <string>

#include <boost/array.hpp>

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
  boost::array<Type, nItens> readArray(lua_State* L, std::string name);

  template <typename Type>
  Type readValue(lua_State* L, std::string name);

  // Return true if the field name exists
  bool hasField(lua_State* L, std::string name);

  void readCamera(lua_State* L);

  void readArtkpTargets(lua_State* L);
};


#endif // CF_PARSER_HPP
