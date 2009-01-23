#include "xmlParser.hpp"

#include <cstddef>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace scene;

template <typename Type, int nItens>
boost::array<Type, nItens>
XmlParser::readElementText(const TiXmlElement* const element) const
{
  if (element == NULL)
  {
    throw std::invalid_argument("XML element pointer in readElementText "
                                "method is NULL.");
  }

  // A lib TinyXml já cuida de colocar todos os itens separados por espaços.
  boost::char_separator<char> separator(" ");
  boost::array<Type, nItens> returnArray;

  std::string text(element->GetText());
  boost::tokenizer< boost::char_separator<char> > tok(text, separator);
  unsigned i = 0;

  BOOST_FOREACH(std::string item, tok)
  {
    try
    {
      returnArray.at(i) = boost::lexical_cast<Type>(item);
    }
    catch(std::range_error e)
    {
      throw xmlLoad_error("Too much items in element " +
                          element->ValueStr() + ". Expected " +
                          boost::lexical_cast<std::string>(nItens) +
                          " items.");
    }
    i++;
  }
  if(i < returnArray.size())
  {
    throw xmlLoad_error("Too few items in element " +
                        element->ValueStr() + ". Expected " +
                        boost::lexical_cast<std::string>(nItens) +
                        " items.");
  }

  return returnArray;
}

template <typename Type>
Type
XmlParser::readAttribute(const TiXmlElement* const element,
                         const std::string& attributeName) const
{
  if(element == NULL)
  {
    throw std::invalid_argument("XML element pointer in readAttribute "
                                "method is NULL.");
  }
  if(element->Attribute(attributeName.c_str()) == NULL)
  {
    throw std::invalid_argument("Attribute " + attributeName +
                                " don't found in element " +
                                element->ValueStr() + ".");
  }

  return boost::lexical_cast<Type>(element->Attribute(attributeName.c_str()));
}


Scene
XmlParser::parseFile(const std::string& xmlFile) const
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
    Camera camera;

    if(readAttribute<std::string>(hElemCamera.ToElement(), "on")
       != "yes")
    {
      continue;
    }

    camera.hardware =
      readElementText<std::string,1>(hElemCamera.
                                     FirstChildElement("hardware").
                                     ToElement()).at(0);
    boost::to_lower(camera.hardware);
    if(camera.hardware == "pseudocamera")
    {
      camera.imagesPath =
        readAttribute<std::string>(hElemCamera.
                                   FirstChildElement("hardware").
                                   ToElement(), "path");
    }

    camera.uid =
      readElementText<std::string,1>(hElemCamera.
                                     FirstChildElement("uid").
                                     ToElement()).at(0);

    camera.resolution =
      readElementText<unsigned,2>(hElemCamera.
                                  FirstChildElement("resolution").
                                  ToElement());
    camera.frameRate =
      readElementText<float,1>(hElemCamera.
                               FirstChildElement("frame_rate").
                               ToElement()).at(0);

    TiXmlElement* matrixHomography = hElemCamera.
                                     FirstChildElement("matrix_homography").
                                     ToElement();
    TiXmlElement* lutHomography = hElemCamera.
                                  FirstChildElement("lut_homography").
                                  ToElement();
    if (matrixHomography)
    {
      camera.homography = MatrixHomography();

      camera.getMatrixHomography().h =
        readElementText<double,9>(matrixHomography->
                                  FirstChildElement("matrix_h"));
      camera.getMatrixHomography().fc =
        readElementText<double,2>(matrixHomography->
                                  FirstChildElement("focal_length"));
      camera.getMatrixHomography().cc =
        readElementText<double,2>(matrixHomography->
                                  FirstChildElement("principal_point"));
      camera.getMatrixHomography().kc =
        readElementText<double,5>(matrixHomography->
                                  FirstChildElement("radial_distortion"));
      camera.getMatrixHomography().alpha_c =
        readElementText<double,1>(matrixHomography->
                                  FirstChildElement("alpha_c")).at(0);
    }
    else if (lutHomography)
    {
      camera.homography = LutHomography();

      camera.getLutHomography().lut =
        readElementText<std::string,2>(lutHomography->
                                       FirstChildElement("lut"));
    }
    else
    {
      throw xmlLoad_error("Homography don't found in XML scene file");
    }

    scene.vecCamera.push_back(camera);
  }

  TiXmlHandle hTargetsBase(hRoot.FirstChildElement("targets"));
  TiXmlHandle hElemTarget(NULL);
  Target target;
  std::string targetType;
  int targetNum;
  for(targetNum = 0, hElemTarget = hTargetsBase.ChildElement(targetNum);
      hElemTarget.ToElement() != NULL;
      targetNum++, hElemTarget = hTargetsBase.ChildElement(targetNum))
  {
    target.targetDefineFile =
      readAttribute<std::string>(hElemTarget.ToElement(), "file");

    target.uid =
      readElementText<unsigned, 1>(hElemTarget.
                                  FirstChildElement("uid").
                                  ToElement()).at(0);

    targetType = hElemTarget.ToElement()->ValueStr();
    boost::to_lower(targetType);

    scene.targets[targetType].push_back(target);
  }

  return scene;
}
