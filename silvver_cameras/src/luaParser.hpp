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

#ifndef _LUA_PARSER_HPP_
#define _LUA_PARSER_HPP_

#include <boost/function.hpp>
#include <boost/array.hpp>
#include <boost/optional.hpp>
#include <string>
#include <vector>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class LuaParser
{
public:
  LuaParser(const std::string& luaFile);

  ~LuaParser();

  void loadGlobalTable(const std::string& name);

  void loadTable(const std::string& name);

  void unloadTable();

  /** Iterate over all numeric indexes of top table.
   * Iterate over the table currently on top of Lua stack, but only the
   * numeric indexes and in crescent order. Put each item on top of Lua stack,
   * call the callback function, then pop that item.
   *
   * @param callback A callback function that will handle each item. */
  void iterateArray(boost::function<void (void)> callback);

  // Return true if the field name exists
  bool hasField(const std::string& fieldName) const;

  template<class Type>
  void readValue(Type& output, const std::string& fieldName);

  template<class Type>
  void readValue(boost::optional<Type>& output, const std::string& fieldName);

private:
  void getTop(double& output) const;

  void getTop(float& output) const;

  void getTop(int& output) const;

  void getTop(unsigned int& output) const;

  void getTop(bool& output) const;

  void getTop(std::string& output) const;

  template <class Type, std::size_t nItens>
  void getTop(boost::array<Type, nItens>& output);

  template <class Type>
  void getTop(std::vector<Type>& output);

  lua_State* L;
};

#endif // _LUA_PARSER_HPP_
