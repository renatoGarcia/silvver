#include "sceneMounter.hpp"
#include <boost/foreach.hpp>
#include "tsPrint.hpp"

SceneMounter::SceneMounter(const std::string& serverIP,
                           const int receptionistPort,
                           const std::string& xmlSceneDescriptor)
  :xmlParser()
  ,xmlSceneDescriptor(xmlSceneDescriptor)
{
  receptionistConnection.reset(new Connection(serverIP, receptionistPort));
}

void
SceneMounter::mount()
{
  // Está aqui só para compilar, não é usado.
  double tempoInicial = 0;

  Scene scene = this->xmlParser.parseFile(this->xmlSceneDescriptor);
  this->receptionistConnection->connect();

  char msgTP[3] = "TP";
  this->receptionistConnection->send((void*)msgTP, sizeof(msgTP));

  // Recebe o tempo atual no silvver-servidor.
  this->receptionistConnection->receive((char*)&tempoInicial,
                                        sizeof(tempoInicial) );

  std::pair< std::string, std::vector<TargetConfig> > targetTypeGroup;
  CameraConfig cameraConfig;
  // Para cada tipo diferente de alvo
  BOOST_FOREACH(targetTypeGroup, scene.targets)
  {
    // Constrói uma câmera abstrata para cada uma das câmeras físicas
    BOOST_FOREACH(cameraConfig, scene.vecCameraConfig)
    {
      this->constructAbstractCamera(targetTypeGroup.first,
                                    targetTypeGroup.second,
                                    cameraConfig);
    }
  }

}

void
SceneMounter::constructAbstractCamera(std::string targetType,
                                      const std::vector<TargetConfig> &vecTargets,
                                      CameraConfig &cameraConfig)
{
  char msgPT[3] = "PT";
  std::string serverIP = this->receptionistConnection->getPairIP();
  unsigned connectionPort;

  // Envia a primeira mensagem ao recepcionista, uma string "PT".
  this->receptionistConnection->send((void*)msgPT, sizeof(msgPT));

  // Recebe como resposta a porta na qual a câmera deverá se conectar.
  this->receptionistConnection->receive((char*)&connectionPort,
                                        sizeof(unsigned));

  {PRINT_LOCK;
   std::cout << "PORTA: " << connectionPort << std::endl;}

  boost::shared_ptr<AbstractCamera> abstractCameraPtr;
  if(targetType == "artp_mark")
  {
    abstractCameraPtr.reset(new MarkerCamera(vecTargets,
                                             cameraConfig,
                                             serverIP,
                                             connectionPort));
  }
  else if(targetType == "color_blob")
  {
    abstractCameraPtr.reset(new BlobCamera(vecTargets,
                                           cameraConfig,
                                           serverIP,
                                           connectionPort));

  }
  else
  {
    throw std::invalid_argument("Camera abstrata desconhecida: " + targetType);
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
