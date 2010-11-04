/* Copyright 2009-2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#ifndef _ABSTRACT_CAMERA_FACTORY_HPP_
#define _ABSTRACT_CAMERA_FACTORY_HPP_

#include <boost/variant/static_visitor.hpp>

#include "abstractCameras/abstractCamera.hpp"
#include "scene.hpp"

class AbstractCameraFactory
{
public:
  static AbstractCamera*
  create(const scene::AnyHardCamera& hardCamera,
         const scene::AnyTargetSet& targetSets);

private:
  class Visitor
    :public boost::static_visitor<AbstractCamera*>
  {
  public:
    Visitor(const scene::AnyHardCamera& hardCamera);

    AbstractCamera* operator()(const scene::ArtkpTargets& artkpTarget) const;
    AbstractCamera* operator()(const scene::ButterflyTargets& butterflyTarget) const;

  private:
    const scene::AnyHardCamera hardCamera;
  };

  AbstractCameraFactory();
};

#endif /* _ABSTRACT_CAMERA_FACTORY_HPP_ */
