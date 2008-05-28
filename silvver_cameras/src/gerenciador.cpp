#include "gerenciador.hpp"

// Mutex usado para escrever na saída padrão. Declarado em main.cpp.
extern boost::mutex mutexCout;

Gerenciador::Gerenciador(int portaRecepcionista, std::string ipServidor,
                         std::vector<CameraConfig> &vecCameraConfig)
{
  this->vecCameraConfig.resize( vecCameraConfig.size() );
  copy( vecCameraConfig.begin(), vecCameraConfig.end(),
        this->vecCameraConfig.begin() );

  this->portaRecepcionista = portaRecepcionista;
  this->ipServidor = ipServidor;

  conexaoRecepcionista.reset( new Conexao() );
}

void
Gerenciador::RodarGerenciador()
{
  // Está aqui só para compilar, não é usado.
  double tempoInicial = 0;

  this->conexaoRecepcionista->Iniciar(portaRecepcionista, ipServidor.c_str());

  char msgTP[3] = "TP";
  this->conexaoRecepcionista->Enviar( (void*) msgTP,sizeof(msgTP) );

  // Recebe o tempo atual no silvver-servidor.
  this->conexaoRecepcionista->Receber((char*)&tempoInicial,
                                      sizeof(tempoInicial) );

  BOOST_FOREACH(CameraConfig camConf, this->vecCameraConfig)
  {
    ConectarCamera(camConf);
  }
}

void
Gerenciador::ConectarCamera(const CameraConfig &cameraConfig)
{
  char msgPT[3] = "PT";
  unsigned portaConectar;

  // Envia a primeira mensagem ao recepcionista, uma string "PT".
  conexaoRecepcionista->Enviar((void*)msgPT, sizeof(msgPT));

  // Recebe como resposta a porta na qual a câmera deverá se conectar.
  conexaoRecepcionista->Receber((char*)&portaConectar, sizeof(unsigned));

  {boost::mutex::scoped_lock lock(mutexCout);
   std::cout << "PORTA: " << portaConectar << std::endl;}

  switch(cameraConfig.modeloAbstrato)
  {
  case CameraConfig::MARCO:
    {
      vecCamControl.push_back(boost::shared_ptr<Controlador>
       	                      (new MarcoCameraControlador(cameraConfig,
                                                          portaConectar,
                                                          ipServidor)));
      thCamera.push_back(boost::shared_ptr<boost::thread>
                         (new boost::thread( boost::ref(*vecCamControl.back())) ));
      break;
    }
  }
}

void Gerenciador::PararGerenciador()
{
  BOOST_FOREACH(boost::shared_ptr<Controlador> controlador, vecCamControl)
  {
    controlador->terminar();
  }
  BOOST_FOREACH(boost::shared_ptr<boost::thread> thr, thCamera)
  {
    thr->join();
  }
}
