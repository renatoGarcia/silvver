#include "controlador.hpp"
#include <string.h>

extern boost::mutex mutexCout;

mutex Controlador::mutexInicarCamera;
bool Controlador::termina = false;
//Conexao* Controlador::conexaoRecepcionista = NULL;

Controlador::Controlador(int porta,char *ip,TipoDado codigoDado,vector<CameraConfig> &vecCameraConfig)
  :DADO(codigoDado)
{
  this->vecCameraConfig.resize( vecCameraConfig.size() );
  copy( vecCameraConfig.begin(),vecCameraConfig.end(),this->vecCameraConfig.begin() );

  portaReceptor = porta;
  ipReceptor = new char[10];
  strcpy(ipReceptor,ip);

  conexaoRecepcionista = new Conexao();
  conexaoRecepcionista->Iniciar(portaReceptor,ipReceptor);
}

Controlador::~Controlador()
{
  delete conexaoRecepcionista;
}

void Controlador::RodarControlador()
{
  vector<CameraConfig>::iterator iteCameras;
  for(iteCameraConfig = vecCameraConfig.begin(); iteCameraConfig < vecCameraConfig.end(); iteCameraConfig++)
  {
    ConectarCamera(*iteCameraConfig);
  }
}

void Controlador::ConectarCamera(const CameraConfig &cameraConfig)
{
  char msgPT[3] = "PT";
  unsigned portaConectar;
  double tempoInicial;

  // Envia a primeira mensagem ao recepcionista("PT"), para que ele envie
  // como resposta a porta na qual a blobCamera deverá se conectar.
  conexaoRecepcionista->Enviar( (void*) msgPT,sizeof(msgPT) );
  conexaoRecepcionista->Receber( (char*)&portaConectar,sizeof(unsigned) );

  conexaoRecepcionista->Receber( (char*)&tempoInicial, sizeof(tempoInicial) );

  {mutex::scoped_lock lock(mutexCout);
    cout << "PORTA: " << portaConectar << endl;}

  switch(DADO)
  {
  case CORES:
    {
      thCamera.push_back( new thread(bind(&Controlador::BlobCam,
                                          cameraConfig,
                                          vectorCores,
                                          portaConectar,
                                          ipReceptor) ) );
      break;
    }

  case MARCAS:
    {
      thCamera.push_back( new thread(bind(&Controlador::MarcaCam,
                                          cameraConfig,
                                          portaConectar,
                                          ipReceptor,
                                          tempoInicial) ) );

      break;
    }
  }
}

void Controlador::BlobCam(const CameraConfig &cameraConfig,
                          vector<string> vecCores,int porta,char *ip)
{
  BlobCamera *blobCam;
  unsigned blobCamID;

  char msgTempo[3] = "TP"; //Mensagem para obter o tempo atual no servidor.
  {mutex::scoped_lock lock(mutexInicarCamera);

    double tempoInicial;
    //conexaoRecepcionista->Enviar( (void*) msgTempo,sizeof(msgTempo) );
    //conexaoRecepcionista->Receber( (char*)&tempoInicial, sizeof(tempoInicial) );

    blobCam = new BlobCamera(cameraConfig,tempoInicial);
    blobCamID = blobCam->Iniciar();
  }

  Conexao conexao;
  conexao.Iniciar(porta,ip);

  // Envia a segunda mensagem ao recepcionista("OK"), informando que que a conexao foi criada
  char msgOK[3] = "OK";
  conexao.Enviar( msgOK,sizeof(msgOK) );

  // Evia a terceira mensagem ao recepcionista, informando o c�digo do dado que ser� transmitido
  int tipoDado = CORES;
  conexao.Enviar( &tipoDado,sizeof(tipoDado) );

  char msg[3];
  // Espera uma mensagem de confirma��o
  conexao.Receber( msg,sizeof(msg) );
  {mutex::scoped_lock lock(mutexCout);
    cout << "CAMERA: " << blobCamID << " " << msg << endl;}

  vector<Marca> vecMarca;
  Pacote<Marca> pacote(blobCamID);

  while(!termina)
  {
    blobCam->ProcessarImagem(vecMarca);
    pacote.Empacotar(vecMarca);
    conexao.Enviar( &pacote,sizeof(pacote) );
  }

  delete blobCam;

}

void Controlador::MarcaCam(const CameraConfig &cameraConfig,int porta,char *ip,double tempoInicial)
{
  MarcoCamera *marcaCam;
  unsigned marcaCamID;

  {mutex::scoped_lock lock(mutexInicarCamera);
    marcaCam = new MarcoCamera(cameraConfig,tempoInicial);
    marcaCamID = marcaCam->Iniciar();}

  Conexao conexao;
  conexao.Iniciar(porta,ip);

  // Envia a segunda mensagem ao recepcionista("OK"), informando que que a conexao foi criada
  char msgOK[3] = "OK";
  conexao.Enviar( msgOK,sizeof(msgOK) );

  // Evia a terceira mensagem ao recepcionista, informando o c�digo do dado que ser� transmitido
  int tipoDado = MARCAS;
  conexao.Enviar( &tipoDado,sizeof(tipoDado) );

  char msg[3];
  // Espera uma mensagem de confirma��o
  conexao.Receber( msg,sizeof(msg) );
  {mutex::scoped_lock lock(mutexCout);
    cout << "CAMERA: " << marcaCamID << " " << msg << endl;}

  vector<Ente> vecEnte;
  Pacote<Ente> pacote(marcaCamID);

  while(!termina)
  {
    marcaCam->ProcessarImagem(vecEnte);
    pacote.Empacotar(vecEnte);
    conexao.Enviar( &pacote,sizeof(pacote) );
  }

  delete marcaCam;
}

void Controlador::PararControlador()
{
  vector< thread* >::iterator iteThread,fimVecThread;
  iteThread = thCamera.begin();
  fimVecThread = thCamera.end();

  termina = true;
  for(;iteThread!=fimVecThread; iteThread++)
  {
    (*iteThread)->join();
  }
}
