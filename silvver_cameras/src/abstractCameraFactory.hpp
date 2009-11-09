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

#ifndef ABSTRACT_CAMERA_FACTORY_HPP
#define ABSTRACT_CAMERA_FACTORY_HPP

#include <vector>
#include <string>

#include "abstractCameras/abstractCamera.hpp"
#include "connection.hpp"
#include "scene.hpp"

class AbstractCameraFactory
{
public:

  static AbstractCamera*
  create(const scene::Camera& cameraConfig,
         const scene::AnyTarget& targets,
         boost::shared_ptr<Connection> connection,
         const bool showImages);

private:

  AbstractCameraFactory();
};


#endif /* ABSTRACT_CAMERA_FACTORY_HPP */
