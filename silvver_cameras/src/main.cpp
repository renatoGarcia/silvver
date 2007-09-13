#include <iostream>
#include <string>
#include <string.h>
#include "controlador.h"
#include "camera.h"
#include "tinyXml/tinyxml.h"
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;

// Mutex para controlar a escrita na saida padrão
boost::mutex mutexCout;

void LerDadosCameras(vector<CameraConfig> &vecCameraConfig);

int main(int argc, char **argv)
{

  char ip[16]="127.0.0.1";
  int portaRecepcionista = 12000;

  if(argc>1)
  {
    strncpy(ip,argv[1],16);
  }
  if(argc>2)
  {
    portaRecepcionista = atoi(argv[2]);
  }

  TipoDado tipoDado = /*CORES*/MARCAS;

  //-------------------------------------------------------------//

  vector<CameraConfig> vecCameraConfig;
  LerDadosCameras(vecCameraConfig);

  Controlador *controlador;
  controlador = new Controlador(portaRecepcionista,ip,tipoDado,vecCameraConfig);

  controlador->RodarControlador();

  char ch = getchar(); //Espera o usuário teclar enter para terminar.

  cout << "Terminando..." << endl;

  controlador->PararControlador();
  delete controlador;

  cout << "Fim do Programa" << endl;

  return 0;
}

void LerDadosCameras(vector<CameraConfig> &vecCameraConfig)
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

      TiXmlElement *elemModelo = hCamera.FirstChild("modelo").ToElement();
      if ( elemModelo )
      {
	 sstream.clear();
	 sstream.str(elemModelo->GetText());
	 if( sstream.str() == "PGR" )tempCamConf.modelo = CameraConfig::PGR;
	 else tempCamConf.modelo = CameraConfig::PseudoCam;
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
	  sstream.str(elemDistFocal->GetText())
            sstream >> tempCamConf.fc[0] >> tempCamConf.fc[1];       
        }

      TiXmlElement *elemPontoPrincipal = hCamera.FirstChild("ponto_principal").ToElement();
      if( elemPontoPrincipal )
        {
	  sstream.clear();
	  sstream.str(elemPontoPrincipal->GetText())
            sstream >> tempCamConf.cc[0] >> tempCamConf.cc[1];       
        }

      TiXmlElement *elemDistorcaoRadial = hCamera.FirstChild("distorcao_radial").ToElement();
      if( elemDistorcaoRadial )
        {
	  sstream.clear();
	  sstream.str(elemDistorcaoRadial->GetText())
            sstream >> tempCamConf.kc[0] >> tempCamConf.kc[1] >> tempCamConf.kc[2] 
                    >> tempCamConf.kc[3] >> tempCamConf.kc[4];       
        }

      TiXmlElement *elemAlfaC = hCamera.FirstChild("alfa_c").ToElement();
      if( elemAlfaC )
        {
	  sstream.clear();
	  sstream.str(elemAlfaC->GetText())
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
