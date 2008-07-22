#include "gerenciador.hpp"

// Mutex usado para escrever na saída padrão. Declarado em main.cpp.
extern boost::mutex mutexCout;

Gerenciador::Gerenciador(int portaRecepcionista, std::string ipServidor,
                         Scene scene)
{
  this->scene = scene;
  this->portaRecepcionista = portaRecepcionista;
  this->ipServidor = ipServidor;

  conexaoRecepcionista.reset(new Connection(ipServidor, portaRecepcionista));
}

void
Gerenciador::RodarGerenciador()
{
  // Está aqui só para compilar, não é usado.
  double tempoInicial = 0;

  this->conexaoRecepcionista->connect();

  char msgTP[3] = "TP";
  this->conexaoRecepcionista->send( (void*) msgTP,sizeof(msgTP) );

  // Recebe o tempo atual no silvver-servidor.
  this->conexaoRecepcionista->receive((char*)&tempoInicial,
                                      sizeof(tempoInicial) );

  BOOST_FOREACH(CameraConfig camConf, this->scene.vecCameraConfig)
  {
    if(this->scene.vecARTPMarks.empty() == false)
    {
      ConectarCamera(camConf, this->scene.vecARTPMarks,
                     AbstractCamera::ARTP_MARK);
    }

    if(this->scene.vecColorBlobs.empty() == false)
    {
      ConectarCamera(camConf, this->scene.vecColorBlobs,
                     AbstractCamera::COLOR_BLOB);
    }

  }
}

void
Gerenciador::ConectarCamera(CameraConfig &cameraConfig,
                            const std::vector<TargetConfig> &vecTargets,
                            AbstractCamera::TargetType targetType)
{
  char msgPT[3] = "PT";
  unsigned portaConectar;

  // Envia a primeira mensagem ao recepcionista, uma string "PT".
  this->conexaoRecepcionista->send((void*)msgPT, sizeof(msgPT));

  // Recebe como resposta a porta na qual a câmera deverá se conectar.
  this->conexaoRecepcionista->receive((char*)&portaConectar, sizeof(unsigned));

  {boost::mutex::scoped_lock lock(mutexCout);
   std::cout << "PORTA: " << portaConectar << std::endl;}

  switch(targetType)
  {
  case AbstractCamera::ARTP_MARK:
    {
      this->vecAbstractCamera.push_back(boost::shared_ptr<AbstractCamera>
                                        (new MarkerCamera
                                         (cameraConfig,
                                          this->ipServidor,
                                          portaConectar)));
      thCamera.push_back(boost::shared_ptr<boost::thread>
                         (new boost::thread(boost::ref
                                            (*vecAbstractCamera.back()))));
      break;
    }
  case AbstractCamera::COLOR_BLOB:
    break;
  }
}

void Gerenciador::PararGerenciador()
{
  BOOST_FOREACH(boost::shared_ptr<AbstractCamera> abstractCamera,
                this->vecAbstractCamera)
  {
    abstractCamera->stop();
  }
  BOOST_FOREACH(boost::shared_ptr<boost::thread> thr, this->thCamera)
  {
    thr->join();
  }
}
