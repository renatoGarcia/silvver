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

#include "abstractCameraFactory.hpp"

#include <boost/variant/apply_visitor.hpp>

#include "exceptions.hpp"

#ifdef HAS_ARTKP
#  include "abstractCameras/artkpCamera.hpp"
#endif
#ifdef HAS_BUTTERFLY
#  include "abstractCameras/butterflyCamera.hpp"
#endif

AbstractCamera*
AbstractCameraFactory::create(const scene::AnyHardCamera& hardCamera,
                              const scene::AnyTargetSet& targetSets)
{
  return boost::apply_visitor(Visitor(hardCamera), targetSets);
}

AbstractCameraFactory::Visitor::
Visitor(const scene::AnyHardCamera& hardCamera)
  :hardCamera(hardCamera)
{}

AbstractCamera*
AbstractCameraFactory::Visitor::
operator()(const scene::ArtkpTargets& artkpTarget) const
{
#ifdef HAS_ARTKP
  return new ArtkpCamera(this->hardCamera, artkpTarget);
#else
  throw invalid_argument()
    << info_what("This program don't was compiled with support "
                 "to ARToolKitPlus targets");
#endif
}

AbstractCamera*
AbstractCameraFactory::Visitor::
operator()(const scene::ButterflyTargets& butterflyTarget) const
{
#ifdef HAS_BUTTERFLY
  return new ButterflyCamera(this->hardCamera, butterflyTarget);
#else
  throw invalid_argument()
    << info_what("This program don't was compiled with support "
                 "to Butterfly targets");
#endif
}
