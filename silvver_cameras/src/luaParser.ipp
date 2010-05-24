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

#ifndef _LUA_PARSER_IPP_
#define _LUA_PARSER_IPP_

#include "luaParser.hpp"

#include "exceptions.hpp"

template<class Type>
void
LuaParser::readValue(Type& output, const std::string& fieldName)
{
  lua_getfield(this->L, -1, fieldName.c_str());
  try
  {
    getTop(output);
    lua_pop(this->L, 1);
  }
  catch (file_parsing_error& e)
  {
    lua_pop(this->L, 1); // Pop the current failed field
    throw e << info_fieldName(fieldName);
  }
}

template <class Type>
void
LuaParser::readValue(boost::optional<Type>& output,const std::string& fieldName)
{
  lua_getfield(this->L, -1, fieldName.c_str());
  try
  {
    Type tmp;
    getTop(tmp);
    output = tmp;
    lua_pop(this->L, 1);
  }
  catch(missing_field& e)
  {
    output.reset();
    lua_pop(this->L, 1);
  }
  catch(type_error& e)
  {
    lua_pop(this->L, 1);
    throw;
  }
}

template <class Type, std::size_t nItens>
void
LuaParser::getTop(boost::array<Type, nItens>& output)
{
  if (lua_isnil(this->L, -1))
  {
    throw missing_field() << info_what("Missing field");
  }
  if (!lua_istable(this->L, -1))
  {
    throw type_error() << info_what("Field is not an array");
  }

  for (std::size_t i = 1; i <= nItens; ++i)
  {
    lua_pushnumber(this->L, i);
    lua_gettable(this->L, -2);

    try
    {
      getTop(output.at(i-1));
      lua_pop(this->L, 1); // Pop the current item
    }
    catch (file_parsing_error& e)
    {
      lua_pop(this->L, 1); // Pop the current failed item
      throw e << info_arrayIndex(i);
    }
  }
}

template <class Type>
void
LuaParser::getTop(std::vector<Type>& output)
{
  if (lua_isnil(this->L, -1))
  {
    throw missing_field() << info_what("Missing field");
  }
  if (!lua_istable(this->L, -1))
  {
    throw type_error() << info_what("Field is not an array");
  }

  Type tmpValue;
  bool allReaded = false;
  unsigned i = 1;
  do
  {
    lua_pushnumber(this->L, i);
    lua_gettable(this->L, -2);

    try
    {
      getTop(tmpValue);
      output.push_back(tmpValue);
      lua_pop(this->L, 1); // Pop the current item
      ++i;
    }
    catch (missing_field& e)
    {
      lua_pop(this->L, 1); // Pop the current nil item
      allReaded = true; // If found nil con
    }
    catch (file_parsing_error& e)
    {
      lua_pop(this->L, 1); // Pop the current failed item
      throw e << info_arrayIndex(i);
    }
  } while (!allReaded);
}

#endif /* _LUA_PARSER_IPP_ */
