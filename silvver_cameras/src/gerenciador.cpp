#include "gerenciador.hpp"

// Mutex usado para escrever na saída padrão. Declarado em main.cpp.
extern boost::mutex mutexCout;

SceneMounter::SceneMounter(int receptionistPort, std::string serverIP,
                           std::string xmlSceneDescriptor)
  :xmlSceneDescriptor(xmlSceneDescriptor)
  ,receptionistPort(receptionistPort)
  ,serverIP(serverIP)
{
  receptionistConnection.reset(new Connection(serverIP, receptionistPort));
}

void
SceneMounter::mount()
{
  // Está aqui só para compilar, não é usado.
  double tempoInicial = 0;

  Scene scene;
  scene = xmlParser.parseFile(this->xmlSceneDescriptor);

  this->receptionistConnection->connect();

  char msgTP[3] = "TP";
  this->receptionistConnection->send((void*)msgTP, sizeof(msgTP));

  // Recebe o tempo atual no silvver-servidor.
  this->receptionistConnection->receive((char*)&tempoInicial,
                                        sizeof(tempoInicial) );

  BOOST_FOREACH(CameraConfig camConf, scene.vecCameraConfig)
  {
    if(scene.vecARTPMarks.empty() == false)
    {
      this->constructAbstractCamera(camConf, scene.vecARTPMarks,
                                    AbstractCamera::ARTP_MARK);
    }

    if(scene.vecColorBlobs.empty() == false)
    {
      this->constructAbstractCamera(camConf, scene.vecColorBlobs,
                                    AbstractCamera::COLOR_BLOB);
    }

  }
}

void
SceneMounter::constructAbstractCamera(CameraConfig &cameraConfig,
                                      const std::vector<TargetConfig> &vecTargets,
                                      AbstractCamera::TargetType targetType)
{
  char msgPT[3] = "PT";
  unsigned connectionPort;

  // Envia a primeira mensagem ao recepcionista, uma string "PT".
  this->receptionistConnection->send((void*)msgPT, sizeof(msgPT));

  // Recebe como resposta a porta na qual a câmera deverá se conectar.
  this->receptionistConnection->receive((char*)&connectionPort,
                                        sizeof(unsigned));

  {boost::mutex::scoped_lock lock(mutexCout);
   std::cout << "PORTA: " << connectionPort << std::endl;}

  boost::shared_ptr<AbstractCamera> abstractCameraPtr;
  switch(targetType)
  {
  case AbstractCamera::ARTP_MARK:
    {
      abstractCameraPtr.reset(new MarkerCamera(cameraConfig,
                                               this->serverIP,
                                               connectionPort));
      break;
    }
  case AbstractCamera::COLOR_BLOB:
    break;
  }

  this->vecAbstractCamera.push_back(abstractCameraPtr);

  abstractCameraPtr->run();
}

void SceneMounter::dismount()
{
  BOOST_FOREACH(boost::shared_ptr<AbstractCamera> abstractCamera,
                this->vecAbstractCamera)
  {
    abstractCamera->stop();
  }
}
