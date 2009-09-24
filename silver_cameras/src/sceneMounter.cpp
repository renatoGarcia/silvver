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
SceneMounter::mount(const bool showImages)
{
  CfParser cfParser;
  const scene::Scene scene = cfParser.parseFile(this->sceneDescriptorFile);

  scene::Camera camera;
  BOOST_FOREACH(camera, scene.cameras)
  {
    // If there are ARToolKitPlus targets.
    if(scene.targets.get<0>())
    {
      this->constructAbstractCamera(camera,
                                    *scene.targets.get<0>(),
                                    "artp_mark",
                                    showImages);
    }
  }
}

void
SceneMounter::constructAbstractCamera(const scene::Camera& camera,
                                      const scene::VariantAnyTarget& targets,
                                      const std::string& targetType,
                                      const bool showImages)
{
  boost::shared_ptr<Connection>
    connection(new Connection(this->serverIp, this->receptionistPort));

  connection->connect(targetType);

  this->abstractCameras.push_back(AbstractCameraFactory::create(camera,
                                                                targets,
                                                                connection,
                                                                showImages));

  this->abstractCameras.back().run();
}

void SceneMounter::dismount()
{
  BOOST_FOREACH(AbstractCamera& abstractCamera,
                this->abstractCameras)
  {
    abstractCamera.stop();
  }
}
