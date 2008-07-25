#ifndef SCENE_MOUNTER_HPP
#define SCENE_MOUNTER_HPP

#include <vector>
#include <string>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>
#include "abstractCamera.hpp"
#include "markerCamera.hpp"
#include "blobCamera.hpp"
#include "xmlParser.hpp"
#include "connection.hpp"
#include "timer.hpp"

class SceneMounter
{
 public:

  SceneMounter(int receptionistPort, std::string serverIP,
               std::string xmlSceneDescriptor);

  /// Inicia todas as câmeras abstradas contidas no vetod vecCameraConfig.
  void mount();

  /// Fecha todas as câmeras abstratas, e prepara o gerenciador para terminar.
  void dismount();

private:

  XmlParser xmlParser;

  std::string xmlSceneDescriptor;

  /// Threads nas quais os métodos controladores das câmeras serão executados.
//   std::vector< boost::shared_ptr< boost::thread > > thCamera;

  /// Objeto da classe Conexao que se encontra que possui os dados para comunição com o recepcionista do silvver-servidor.
  boost::scoped_ptr<Connection> receptionistConnection;

  // Porta para qual enviar a primeira mensagem, a fim de se
  // descobrir em qual porta a BlobCamera deve se conectar
  int receptionistPort;

  // Endereço ip do servidor
  std::string serverIP;

  /// Timer que sincronizado com tempo do servidor
  boost::scoped_ptr<Timer> timer;

  // Inicia a câmera abstrata descrita por cameraConfig
  void constructAbstractCamera(CameraConfig &cameraConfig,
                               const std::vector<TargetConfig> &vecTargets,
                               AbstractCamera::TargetType targetType);

  std::vector< boost::shared_ptr<AbstractCamera> > vecAbstractCamera;
};

#endif
