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

#include "scene.hpp"
#include "tinyXml/tinyxml.h"

class XmlParser
{
public:

  class xmlLoad_error : public std::logic_error
  {
  public:
    xmlLoad_error(const std::string& whatArg)
      :logic_error(whatArg){};
  };

  scene::Scene parseFile(const std::string& xmlFile) const;

private:

  template <typename Type, int nItens>
  boost::array<Type, nItens>
  readElementText(const TiXmlElement* const element) const;

  template <typename Type>
  Type readAttribute(const TiXmlElement* const element,
                     const std::string& attributeName) const;
};

#endif /* XML_PARSER_HPP */
