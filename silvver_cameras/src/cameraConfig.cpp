#include "cameraConfig.hpp"
#include "tinyXml/tinyxml.h"
#include <sstream>

CameraConfig::CameraConfig()
{}

CameraConfig::CameraConfig(const CameraConfig& camConfig)
{
  this->fc[0] = camConfig.fc[0];
  this->fc[1] = camConfig.fc[1];

  this->cc[0] = camConfig.cc[0];
  this->cc[1] = camConfig.cc[1];

  this->kc[0] = camConfig.kc[0];
  this->kc[1] = camConfig.kc[1];
  this->kc[2] = camConfig.kc[2];
  this->kc[3] = camConfig.kc[3];
  this->kc[4] = camConfig.kc[4];

  this->alpha_c = camConfig.alpha_c;

  this->resolucao[0] = camConfig.resolucao[0];
  this->resolucao[1] = camConfig.resolucao[1];

  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++)
      this->H[i][j] = camConfig.H[i][j];

  this->serial = camConfig.serial;
  this->frequencia = camConfig.frequencia;
  this->diretorio = camConfig.diretorio;
  this->modeloFisico = camConfig.modeloFisico;
  this->modeloAbstrato = camConfig.modeloAbstrato;
}

void CameraConfig::LerDadosCameras(vector<CameraConfig> &vecCameraConfig)
{
  vecCameraConfig.clear();

  string strLig;
  stringstream sstream;
  CameraConfig tempCamConf;
  TiXmlHandle hCamera(0);
  TiXmlElement *elemCamera;

  TiXmlDocument doc("data/cameras.xml");
  if (!doc.LoadFile()){cout<<"Falhou"<<endl; return;}

  TiXmlHandle hdoc(&doc);

  elemCamera = hdoc.FirstChildElement("camera").ToElement();
  for(int i=0; elemCamera!=NULL; i++,elemCamera = hdoc.ChildElement("camera",i).ToElement())
  {
    strLig = elemCamera->Attribute("ligada");
    if( strLig != "sim" ) continue;

    hCamera = TiXmlHandle(elemCamera);

    TiXmlElement *elemModeloFisico = hCamera.FirstChild("modelo_fisico").ToElement();
    if ( elemModeloFisico )
    {
      sstream.clear();
      sstream.str(elemModeloFisico->GetText());
      if( sstream.str() == "PGR" )tempCamConf.modeloFisico = CameraConfig::PGR;
      else tempCamConf.modeloFisico = CameraConfig::PseudoCam;
    }

    TiXmlElement *elemModeloAbstrato = hCamera.FirstChild("modelo_abstrato").ToElement();
    if ( elemModeloAbstrato )
    {
      sstream.clear();
      sstream.str(elemModeloAbstrato->GetText());
      if( sstream.str() == "BLOB" )tempCamConf.modeloAbstrato = CameraConfig::BLOB;
      else tempCamConf.modeloAbstrato = CameraConfig::MARCO;
    }

    TiXmlElement *elemSerial = hCamera.FirstChild("serial").ToElement();
    if( elemSerial )
    {
      sstream.clear();
      sstream.str(elemSerial->GetText());
      sstream >> tempCamConf.serial;
    }

    TiXmlElement *elemDiretorio = hCamera.FirstChild("diretorio").ToElement();
    if( elemDiretorio )
    {
      sstream.clear();
      sstream.str(elemDiretorio->GetText());
      tempCamConf.diretorio = new char[sstream.str().size()+1];
      sstream >> tempCamConf.diretorio;
    }

    TiXmlElement *elemResolucao = hCamera.FirstChild("resolucao").ToElement();
    if( elemResolucao )
    {
      sstream.clear();
      sstream.str(elemResolucao->GetText());
      sstream >> tempCamConf.resolucao[0] >> tempCamConf.resolucao[1];
    }

    TiXmlElement *elemFrequencia = hCamera.FirstChild("frequencia").ToElement();
    if( elemFrequencia )
    {
      sstream.clear();
      sstream.str(elemFrequencia->GetText());
      sstream >> tempCamConf.frequencia;
    }

    TiXmlElement *elemMatrizH = hCamera.FirstChild("matrizH").ToElement();
    if( elemMatrizH )
    {
      sstream.clear();
      sstream.str(elemMatrizH->GetText());
      sstream >> tempCamConf.H[0][0] >> tempCamConf.H[0][1] >> tempCamConf.H[0][2]
	      >> tempCamConf.H[1][0] >> tempCamConf.H[1][1] >> tempCamConf.H[1][2]
	      >> tempCamConf.H[2][0] >> tempCamConf.H[2][1] >> tempCamConf.H[2][2];
    }

    TiXmlElement *elemDistFocal = hCamera.FirstChild("distancia_focal").ToElement();
    if( elemDistFocal )
    {
      sstream.clear();
      sstream.str(elemDistFocal->GetText());
      sstream >> tempCamConf.fc[0] >> tempCamConf.fc[1];
    }

    TiXmlElement *elemPontoPrincipal = hCamera.FirstChild("ponto_principal").ToElement();
    if( elemPontoPrincipal )
    {
      sstream.clear();
      sstream.str(elemPontoPrincipal->GetText());
      sstream >> tempCamConf.cc[0] >> tempCamConf.cc[1];
    }

    TiXmlElement *elemDistorcaoRadial = hCamera.FirstChild("distorcao_radial").ToElement();
    if( elemDistorcaoRadial )
    {
      sstream.clear();
      sstream.str(elemDistorcaoRadial->GetText());
      sstream >> tempCamConf.kc[0] >> tempCamConf.kc[1] >> tempCamConf.kc[2]
	      >> tempCamConf.kc[3] >> tempCamConf.kc[4];
    }

    TiXmlElement *elemAlfaC = hCamera.FirstChild("alfa_c").ToElement();
    if( elemAlfaC )
    {
      sstream.clear();
      sstream.str(elemAlfaC->GetText());
      sstream >> tempCamConf.alpha_c;
    }

    vecCameraConfig.push_back(tempCamConf);


  }

  //----------Nome dos arquivos de cores-------------//

  /*ifstream streamCores("Cores.txt", ios::in);
    string strCores;

    while( streamCores.peek()!= EOF )
    {
    streamCores >> strCores;
    if (strCores.at(0) == '#') continue;
    vectorCores.push_back(strCores);
    } */
}
