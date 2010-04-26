/* Copyright 2009, 2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#include "cfParser.hpp"
#include "abstractCameraFactory.hpp"
#include "log.hpp"
#include "exceptions.hpp"

SceneMounter::SceneMounter(const std::string& sceneDescriptorFile)
  :sceneDescriptorFile(sceneDescriptorFile)
{}

 SceneMounter::~SceneMounter()
{}

void
SceneMounter::mount()
{
  try
  {
    CfParser cfParser;
    const scene::Scene sc = cfParser.parseFile(this->sceneDescriptorFile);

    scene::Camera camera;
    scene::AnyTarget target;
    BOOST_FOREACH(camera, sc.cameras)
    {
      BOOST_FOREACH(target, sc.targets)
      {
        this->constructAbstractCamera(camera, target);
      }
    }
  }
  catch(const file_parsing_error& e)
  {
    message(LogLevel::ERROR)
      << "[Config file parsing error]\n" << errorsInfo2string(e) << std::endl;
    exit(EXIT_FAILURE);
  }
  catch(const silvver_cameras_exception& e)
  {
    message(LogLevel::ERROR)
      << "[Fatal error]\n" << errorsInfo2string(e) << std::endl;
    exit(EXIT_FAILURE);
  }
}

void
SceneMounter::constructAbstractCamera(const scene::Camera& camera,
                                      const scene::AnyTarget& target)
{
  this->abstractCameras.push_back(AbstractCameraFactory::create(camera,
                                                                target));

  this->abstractCameras.back().run();
}
