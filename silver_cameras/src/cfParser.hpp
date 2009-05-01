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
};


#endif // CF_PARSER_HPP
