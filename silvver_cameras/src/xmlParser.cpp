#include "xmlParser.hpp"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

template <typename Type,int nItens>
boost::array<Type, nItens>
XmlParser::readElementText(const TiXmlElement *element)
{
  if(element == NULL)
  {
    throw std::invalid_argument("Elemento requisitado para ler conteúdo não encontrado.");
  }

  // A lib TinyXml trata de colocar todos os itens separados por espaços.
  boost::char_separator<char> separator(" ");
  boost::array<Type, nItens> returnArray;

  std::string text(element->GetText());
  boost::tokenizer<boost::char_separator<char> > tok(text, separator);
  unsigned i = 0;

  BOOST_FOREACH(std::string item, tok)
  {
    try
    {
      returnArray.at(i) = boost::lexical_cast<Type>(item);
    }
    catch(std::range_error e)
    {
      throw XmlLoad_error("Excesso de itens no elemento " +
                          element->ValueStr() + ". Esperado(s) " +
                          boost::lexical_cast<std::string>(nItens) +
                          " item(s).");
    }
    i++;
  }
  if(i < returnArray.size())
  {
    throw XmlLoad_error("Falta de itens no elemento " +
                        element->ValueStr() + ". Esperado(s) " +
                        boost::lexical_cast<std::string>(nItens) +
                        " item(s).");
  }

  return returnArray;
}

template <typename Type>
Type
XmlParser::readAttribute(const TiXmlElement *element,
                         std::string attributeName)
{
  if(element == NULL)
  {
    throw std::invalid_argument("Elemento requisitado para ler atributo não encontrado");
  }
  if(element->Attribute(attributeName.c_str()) == NULL)
  {
    throw std::invalid_argument("Atributo " + attributeName +
                                " não encontrado no elemento " +
                                element->ValueStr() + ".");
  }

  return boost::lexical_cast<Type>(element->Attribute(attributeName.c_str()));
}


Scene
XmlParser::parseFile(std::string xmlFile)
{
  Scene scene;

  TiXmlDocument doc(xmlFile);
  if (!doc.LoadFile())
  {
    throw std::invalid_argument("Erro ao abrir o arquivo XML de configuração.");
  }

  TiXmlHandle hRoot(doc.RootElement());

  TiXmlHandle hElemCamera(NULL);
  int cameraNum;
  for(cameraNum=0,hElemCamera=hRoot.FirstChildElement("camera");
      hElemCamera.ToElement()!=NULL;
      cameraNum++,hElemCamera=hRoot.ChildElement("camera",cameraNum))
  {
    CameraConfig cameraConfig;

    if(readAttribute<std::string>(hElemCamera.ToElement(), "on")
       != "yes")
    {
      continue;
    }

    cameraConfig.hardware =
      readElementText<std::string,1>(hElemCamera.
                                     FirstChildElement("hardware").
                                     ToElement()).at(0);
    boost::to_lower(cameraConfig.hardware);
    if(cameraConfig.hardware == "pseudocamera")
    {
      cameraConfig.imagesPath =
        readAttribute<std::string>(hElemCamera.
                                   FirstChildElement("hardware").
                                   ToElement(), "path");
    }

    cameraConfig.uid =
      readElementText<uint64,1>(hElemCamera.
                                FirstChildElement("uid").
                                ToElement()).at(0);

    cameraConfig.resolution =
      readElementText<unsigned,2>(hElemCamera.
                                  FirstChildElement("resolution").
                                  ToElement());
    cameraConfig.frameRate =
      readElementText<float,1>(hElemCamera.
                               FirstChildElement("frame_rate").
                               ToElement()).at(0);
    cameraConfig.H =
      readElementText<double,9>(hElemCamera.
                                FirstChildElement("matrix_h").
                                ToElement());
    cameraConfig.fc =
      readElementText<double,2>(hElemCamera.
                                FirstChildElement("focal_length").
                                ToElement());
    cameraConfig.cc =
      readElementText<double,2>(hElemCamera.
                                FirstChildElement("principal_point").
                                ToElement());
    cameraConfig.kc =
      readElementText<double,5>(hElemCamera.
                                FirstChildElement("radial_distortion").
                                ToElement());
    cameraConfig.alpha_c =
      readElementText<double,1>(hElemCamera.
                                FirstChildElement("alpha_c").
                                ToElement()).at(0);


    scene.vecCameraConfig.push_back(cameraConfig);
  }

  TiXmlHandle hTargetsBase(hRoot.FirstChildElement("targets"));
  TiXmlHandle hElemTarget(NULL);
  TargetConfig target;
  int targetNum;
  for(targetNum=0,hElemTarget=hTargetsBase.ChildElement(targetNum);
      hElemTarget.ToElement()!=NULL;
      targetNum++,hElemTarget=hTargetsBase.ChildElement(targetNum))
  {
    target.targetDefineFile =
      readAttribute<std::string>(hElemTarget.ToElement(), "file");

    target.uid =
      readElementText<unsigned,1>(hElemTarget.
                                  FirstChildElement("uid").
                                  ToElement()).at(0);

    if(hElemTarget.ToElement()->ValueStr() == "ARTP_marker")
    {
      scene.vecARTPMarks.push_back(target);
    }
    else if(hElemTarget.ToElement()->ValueStr() == "color_blob")
    {
      scene.vecColorBlobs.push_back(target);
    }
    else
    {
      throw XmlLoad_error("Unknown target type: " +
                          hElemTarget.ToElement()->ValueStr());
    }
  }

  return scene;
}
