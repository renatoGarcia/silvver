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
#include <boost/shared_ptr.hpp>

#include "connection.hpp"
#include "cfParser.hpp"
#include "abstractCameraFactory.hpp"

SceneMounter::SceneMounter(const std::string& serverIp,
                           const int receptionistPort,
                           const std::string& sceneDescriptorFile)
  :sceneDescriptorFile(sceneDescriptorFile)
  ,serverIp(serverIp)
  ,receptionistPort(receptionistPort)
{}

 SceneMounter::~SceneMounter()
{}

void
SceneMounter::mount()
{
  CfParser cfParser;
  const scene::Scene scene = cfParser.parseFile(this->sceneDescriptorFile);

  scene::Camera camera;
  scene::AnyTarget target;
  BOOST_FOREACH(camera, scene.cameras)
  {
    BOOST_FOREACH(target, scene.targets)
    {
      this->constructAbstractCamera(camera, target);
    }
  }
}

void
SceneMounter::constructAbstractCamera(const scene::Camera& camera,
                                      const scene::AnyTarget& target)
{
  boost::shared_ptr<Connection>
    connection(new Connection(this->serverIp, this->receptionistPort));

  this->abstractCameras.push_back(AbstractCameraFactory::create(camera,
                                                                target,
                                                                connection));

  this->abstractCameras.back().run();
}
