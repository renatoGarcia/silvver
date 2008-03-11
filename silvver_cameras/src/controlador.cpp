#include "controlador.hpp"
#include <string.h>

extern boost::mutex mutexCout;

mutex Controlador::mutexInicarCamera;
bool Controlador::termina = false;
//Conexao* Controlador::conexaoRecepcionista = NULL;

Controlador::Controlador(int porta,char *ip,vector<CameraConfig> &vecCameraConfig)
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
  vector<CameraConfig>::iterator iteCameraConfig;
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

  // Envia a primeira mensagem ao recepcionista, uma string "PT".
  conexaoRecepcionista->Enviar( (void*) msgPT,sizeof(msgPT) );

  // Recebe como resposta a porta na qual a câmera deverá se conectar.
  conexaoRecepcionista->Receber( (char*)&portaConectar,sizeof(unsigned) );

  // Recebe o tempo atual no silvver-servidor.
  conexaoRecepcionista->Receber( (char*)&tempoInicial, sizeof(tempoInicial) );

  {mutex::scoped_lock lock(mutexCout);
    cout << "PORTA: " << portaConectar << endl;}

  Conexao *conexao = new Conexao();
  conexao->Iniciar(portaConectar,ipReceptor);

  // Envia a segunda mensagem ao recepcionista("OK"), informando que que a conexao foi criada
  char msgOK[3] = "OK";
  conexao->Enviar( msgOK,sizeof(msgOK) );

  // Evia a terceira mensagem ao recepcionista, informando o código do dado que será transmitido
  int tipoDado = cameraConfig.modeloAbstrato;
  conexao->Enviar( &tipoDado,sizeof(tipoDado) );

  char msg[3];
  // Espera uma mensagem de confirmação
  conexao->Receber( msg,sizeof(msg) );
  {mutex::scoped_lock lock(mutexCout);
  cout << "CAMERA: " << cameraConfig.serial << " " << msg << endl;}

  //TODO: tirar isto, usando o que será implementado no cameraConfig
  vector<string> vectorCores;

  switch(cameraConfig.modeloAbstrato)
  {
  case CameraConfig::BLOB:
    {
      thCamera.push_back( new thread(bind(&Controlador::BlobCam,
                                          cameraConfig,
                                          vectorCores,
                                          portaConectar,
                                          ipReceptor,
                                          conexao) ) );
      break;
    }

  case CameraConfig::MARCO:
    {
      thCamera.push_back( new thread(bind(&Controlador::MarcaCam,
                                          cameraConfig,
                                          portaConectar,
                                          ipReceptor,
                                          tempoInicial,
                                          conexao) ) );

      break;
    }
  }
}

void Controlador::BlobCam(const CameraConfig &cameraConfig,vector<string> vecCores,
                          int porta,char *ip,Conexao* conexao)
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

  // Envia a segunda mensagem ao recepcionista("OK"), informando que que a conexao foi criada
  char msgOK[3] = "OK";
  conexao->Enviar( msgOK,sizeof(msgOK) );
  
  vector<Marca> vecMarca;
  Pacote<Marca> pacote(blobCamID);

  while(!termina)
  {
    blobCam->ProcessarImagem(vecMarca);
    pacote.Empacotar(vecMarca);
    conexao->Enviar( &pacote,sizeof(pacote) );
  }

  delete blobCam;

}

void Controlador::MarcaCam(const CameraConfig &cameraConfig,int porta,char *ip,
                           double tempoInicial,Conexao* conexao)
{
  MarcoCamera *marcaCam;
  unsigned marcaCamID;

  {mutex::scoped_lock lock(mutexInicarCamera);
    marcaCam = new MarcoCamera(cameraConfig,tempoInicial);
    marcaCamID = marcaCam->Iniciar();}


  vector<Ente> vecEnte;
  Pacote<Ente> pacote(marcaCamID);

  while(!termina)
  {
    marcaCam->ProcessarImagem(vecEnte);
    pacote.Empacotar(vecEnte);
    conexao->Enviar( &pacote,sizeof(pacote) );
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
