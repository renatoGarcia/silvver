/**
 * @file   xmlParser.hpp
 *
 * @brief  Estrutura e procedimento para representar e ler as
 *         informações sobre as câmeras que serão utilizadas.
 */
#ifndef XML_PARSER_HPP
#define XML_PARSER_HPP

#include <stdexcept>
#include <string>
#include "tinyXml/tinyxml.h"
#include "scene.hpp"

class XmlParser
{
public:

  class XmlLoad_error : std::runtime_error
  {
  public:
    XmlLoad_error(const std::string& whatArg):
      runtime_error(whatArg){};
  };

  Scene parseFile(const std::string& xmlFile) const;

private:

  template <typename Type,int nItens>
  boost::array<Type,nItens> readElementText(const TiXmlElement *element) const;

  template <typename Type>
  Type readAttribute(const TiXmlElement *element,
                     std::string attributeName) const;
};

#endif /* XML_PARSER_HPP */
