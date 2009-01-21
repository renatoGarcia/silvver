#ifndef SCENE_MOUNTER_HPP
#define SCENE_MOUNTER_HPP

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include "abstractCamera.hpp"
#include "markerCamera.hpp"
#include "blobCamera.hpp"
#include "scene.hpp"
#include "xmlParser.hpp"
#include "connection.hpp"

class SceneMounter
{
 public:

  /** sceneMounter class constructor.
   *
   * @param receptionistPort Port where the receptionist of silver-server is hearing.
   * @param serverIP IP address where silver-server is running.
   * @param xmlSceneDescriptor Path to XML file scene descriptor.
   */
  SceneMounter(const std::string& serverIP, const int receptionistPort,
               const std::string& xmlSceneDescriptor);

  /** Initialize all abstract cameras.
   *
   * The abstract cameras that will be initized are described in the scene
   * descriptor XML file.
   */
  void mount();

  /// Close all opened abtract cameras
  void dismount();

private:

  const XmlParser xmlParser;

  const std::string xmlSceneDescriptor;

  /// Connection with receptionist of silver-server
  boost::scoped_ptr<Connection> receptionistConnection;

  // Inicia a câmera abstrata descrita por cameraConfig
  void constructAbstractCamera(std::string targetType,
                               const std::vector<TargetConfig> &vecTargets,
                               CameraConfig &cameraConfig);

  std::vector< boost::shared_ptr<AbstractCamera> > vecAbstractCamera;
};

#endif
