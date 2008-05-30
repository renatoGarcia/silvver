#include "cameraConfig.hpp"

//--------------------------- CAMERA_CONFIG -------------------------------//

CameraConfig::CameraConfig()
{}

CameraConfig::CameraConfig(const CameraConfig& camConfig)
{
  this->fc = camConfig.fc;
  this->cc = camConfig.cc;
  this->kc = camConfig.kc;
  this->alpha_c = camConfig.alpha_c;
  this->resolucao = camConfig.resolucao;
  this->H = camConfig.H;
  this->serial = camConfig.serial;
  this->frameRate = camConfig.frameRate;
  this->diretorio = camConfig.diretorio;
  this->modeloFisico = camConfig.modeloFisico;
  this->modeloAbstrato = camConfig.modeloAbstrato;
}


//------------------------ CAMERA_CONFIG_EXTRACTOR ------------------------//

template <typename Tipo,int nItens>
boost::array<Tipo, nItens>
CameraConfigExtractor::lerConteudoElemento(const TiXmlElement *elemento)
{
  if(elemento == NULL)
  {
    throw std::invalid_argument("Elemento requisitado para ler conteúdo não encontrado.");
  }

  // A lib TinyXml trata de colocar todos os itens separados por espaços.
  boost::char_separator<char> separadores(" ");
  boost::array<Tipo, nItens> arrayRetorno;

  std::string texto(elemento->GetText());
  boost::tokenizer<boost::char_separator<char> > tok(texto, separadores);
  unsigned i = 0;

  BOOST_FOREACH(std::string item, tok)
  {
    try
    {
      arrayRetorno.at(i) = boost::lexical_cast<Tipo>(item);
    }
    catch(std::range_error e)
    {
      throw XmlLoad_error("Excesso de itens no elemento " +
                          elemento->ValueStr() + ". Esperado(s) " +
                          boost::lexical_cast<std::string>(nItens) +
                          " item(s).");
    }
    i++;
  }
  if(i < arrayRetorno.size())
  {
    throw XmlLoad_error("Falta de itens no elemento " +
                        elemento->ValueStr() + ". Esperado(s) " +
                        boost::lexical_cast<std::string>(nItens) +
                        " item(s).");
  }

  return arrayRetorno;
}

template <typename Tipo>
Tipo CameraConfigExtractor::
lerAtributoElemento(const TiXmlElement *elemento,
                    const std::string& nomeAtributo)
{
  if(elemento == NULL)
  {
    throw std::invalid_argument("Elemento requisitado para ler atributo não encontrado");
  }
  if(elemento->Attribute(nomeAtributo.c_str()) == NULL)
  {
    throw std::invalid_argument("Atributo " + nomeAtributo +
                                " não encontrado no elemento " +
                                elemento->ValueStr() + ".");
  }

  return boost::lexical_cast<Tipo>(elemento->Attribute(nomeAtributo.c_str()));
}


std::vector<CameraConfig>
CameraConfigExtractor::lerDadosCameras(const std::string arquivoConfiguracao)
{
  CameraConfig tempCamConf;
  std::vector<CameraConfig> vecCameraConfig;

  TiXmlDocument doc(arquivoConfiguracao);
  if (!doc.LoadFile())
  {
    throw std::invalid_argument("Erro ao abrir o arquivo XML de configuração das câmeras.");
  }

  TiXmlHandle hRoot(doc.RootElement());
  TiXmlHandle hElemCamera(NULL);

  int cameraNum;
  for(cameraNum=0,hElemCamera=hRoot.FirstChildElement("camera");
      hElemCamera.ToElement()!=NULL;
      cameraNum++,hElemCamera=hRoot.ChildElement("camera",cameraNum))
  {
    if(lerAtributoElemento<std::string>(hElemCamera.ToElement(), "ligada")
       != "sim")
    {
      continue;
    }


    std::string modeloFisico =
      lerConteudoElemento<std::string, 1>(hElemCamera.
                                          FirstChildElement("modelo_fisico").
                                          ToElement()).at(0);
    if(modeloFisico == "PGR")
    {
      tempCamConf.modeloFisico = CameraConfig::PGR;
    }
    else if(modeloFisico == "DC1394")
    {
      tempCamConf.modeloFisico = CameraConfig::DC1394;
    }
    else if(modeloFisico == "PseudoCam")
    {
      tempCamConf.modeloFisico = CameraConfig::PseudoCam;
    }
    else
    {
      throw std::invalid_argument("Unknown hardware camera: " + modeloFisico);
    }


    std::string modeloAbstrato =
      lerConteudoElemento<std::string, 1>(hElemCamera.
                                          FirstChildElement("modelo_abstrato").
                                          ToElement()).at(0);
    if(modeloAbstrato == "BLOB" )
    {
      tempCamConf.modeloAbstrato = CameraConfig::BLOB;
    }
    else
    {
      tempCamConf.modeloAbstrato = CameraConfig::MARCO;
    }


    tempCamConf.serial =
      lerConteudoElemento<unsigned, 1>(hElemCamera.
                                       FirstChildElement("serial").
                                       ToElement()).at(0);
    tempCamConf.resolucao =
      lerConteudoElemento<unsigned, 2>(hElemCamera.
                                       FirstChildElement("resolucao").
                                       ToElement());
    tempCamConf.frameRate =
      lerConteudoElemento<float, 1>(hElemCamera.
                                    FirstChildElement("frame_rate").
                                    ToElement()).at(0);
    tempCamConf.H =
      lerConteudoElemento<double, 9>(hElemCamera.
                                     FirstChildElement("matrizH").
                                     ToElement());
    tempCamConf.fc =
      lerConteudoElemento<double, 2>(hElemCamera.
                                     FirstChildElement("distancia_focal").
                                     ToElement());
    tempCamConf.cc =
      lerConteudoElemento<double, 2>(hElemCamera.
                                     FirstChildElement("ponto_principal").
                                     ToElement());
    tempCamConf.kc =
      lerConteudoElemento<double, 5>(hElemCamera.
                                     FirstChildElement("distorcao_radial").
                                     ToElement());
    tempCamConf.alpha_c =
      lerConteudoElemento<double, 1>(hElemCamera.
                                     FirstChildElement("alfa_c").
                                     ToElement()).at(0);

    vecCameraConfig.push_back(tempCamConf);
  }

  return vecCameraConfig;
}
