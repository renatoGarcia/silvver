#include "sceneMounter.hpp"

#include <boost/foreach.hpp>

#include "tsPrint.hpp"
#include "xmlParser.hpp"
#include "abstractCameraFactory.hpp"

SceneMounter::SceneMounter(const std::string& serverIP,
                           const int receptionistPort,
                           const std::string& sceneDescriptorFile)
  :sceneDescriptorFile(sceneDescriptorFile)
{
  receptionistConnection.reset(new Connection(serverIP, receptionistPort));
}

void
SceneMounter::mount()
{
  // Está aqui só para compilar, não é usado.
  double tempoInicial = 0;

  const XmlParser xmlParser = XmlParser();
  const scene::Scene scene = xmlParser.parseFile(this->sceneDescriptorFile);
  this->receptionistConnection->connect();

  char msgTP[3] = "TP";
  this->receptionistConnection->send((void*)msgTP, sizeof(msgTP));

  // Recebe o tempo atual no silvver-servidor.
  this->receptionistConnection->receive((char*)&tempoInicial,
                                        sizeof(tempoInicial) );

  // The std::string is the name of target type, and the std::vector is
  // the set of all the targets of that type.
  std::pair< std::string, std::vector<scene::Target> > targetTypeGroup;
  scene::Camera cameraConfig;

  // For each type of targets...
  BOOST_FOREACH(targetTypeGroup, scene.targets)
  {
    // ...construct an abstract camera for each hardware camera
    BOOST_FOREACH(cameraConfig, scene.vecCamera)
    {
      this->constructAbstractCamera(targetTypeGroup.first,
                                    targetTypeGroup.second,
                                    cameraConfig);
    }
  }
}

void
SceneMounter::constructAbstractCamera(std::string targetType,
                                      const std::vector<scene::Target>& vecTargets,
                                      scene::Camera &cameraConfig)
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

  boost::shared_ptr<AbstractCamera> abstractCameraPtr =
    boost::shared_ptr<AbstractCamera>
    (AbstractCameraFactory::create(targetType,
                                   vecTargets,
                                   cameraConfig,
                                   serverIP,
                                   connectionPort));

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
