/* Copyright 2009 Renato Florentino Garcia <fgar.renato@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sceneMounter.hpp"

#include <boost/foreach.hpp>

#include "cfParser.hpp"
#include "abstractCameraFactory.hpp"
#include <request.hpp>

SceneMounter::SceneMounter(const std::string& serverIp,
                           const int receptionistPort,
                           const std::string& sceneDescriptorFile)
  :sceneDescriptorFile(sceneDescriptorFile)
  ,serverIp(serverIp)
  ,receptionistPort(receptionistPort)
{}

void
SceneMounter::mount()
{
  CfParser cfParser = CfParser();
  const scene::Scene scene = cfParser.parseFile(this->sceneDescriptorFile);

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
SceneMounter::constructAbstractCamera(const std::string& targetType,
                                      const std::vector<scene::Target>& vecTargets,
                                      scene::Camera& cameraConfig)
{
  boost::shared_ptr<Connection>
    connection(new Connection(this->serverIp, this->receptionistPort));

  connection->connect(targetType);

  boost::shared_ptr<AbstractCamera>
    abstractCameraPtr(AbstractCameraFactory::create(targetType,
                                                    vecTargets,
                                                    cameraConfig,
                                                    connection));

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
