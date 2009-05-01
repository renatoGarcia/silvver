#ifndef SCENE_MOUNTER_HPP
#define SCENE_MOUNTER_HPP

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>

#include "abstractCamera.hpp"
#include "connection.hpp"
#include "scene.hpp"

class SceneMounter
{
public:

  /** sceneMounter class constructor.
   *
   * @param receptionistPort Port where the receptionist of silver-server is hearing.
   * @param serverIP IP address where silver-server is running.
   * @param sceneDescriptorFile Path to XML file scene descriptor.
   */
  SceneMounter(const std::string& serverIp, const int receptionistPort,
               const std::string& sceneDescriptorFile);

  /** Initialize all abstract cameras.
   *
   * The abstract cameras that will be initized are described in the scene
   * descriptor XML file.
   */
  void mount();

  /// Close all opened abtract cameras
  void dismount();

private:

  const std::string sceneDescriptorFile;

  const std::string serverIp;
  const unsigned receptionistPort;

  // Initialize the abstract camera given by cameraConfig
  void constructAbstractCamera(const std::string& targetType,
                               const std::vector<scene::Target> &vecTargets,
                               scene::Camera& cameraConfig);

  std::vector< boost::shared_ptr<AbstractCamera> > vecAbstractCamera;
};

#endif //SCENE_MOUNTER_HPP