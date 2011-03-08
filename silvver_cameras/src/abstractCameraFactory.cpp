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

#include "abstractCameraFactory.hpp"

#include <boost/variant/get.hpp>

#include "exceptions.hpp"

#ifdef HAS_ARTKP
#  include "abstractCameras/artkpCamera.hpp"
#endif
#ifdef HAS_BUTTERFLY
#  include "abstractCameras/butterflyCamera.hpp"
#endif

AbstractCamera*
AbstractCameraFactory::create(const scene::Camera& cameraConfig,
                              const scene::AnyTarget& targets)
{
  AbstractCamera* abstractCamera;

  if (const scene::ArtkpTargets* const artkpTarget =
      boost::get<scene::ArtkpTargets>(&targets))
  {
#ifdef HAS_ARTKP
    abstractCamera = new ArtkpCamera(cameraConfig,
                                     *artkpTarget);
#else
    throw ::invalid_argument()
      << info_what("This program don't was compiled with support "
                   "to ARToolKitPlus targets");
#endif
  }
  else if (const scene::ButterflyTargets* const butterflyTarget =
      boost::get<scene::ButterflyTargets>(&targets))
  {
#ifdef HAS_BUTTERFLY
    abstractCamera = new ButterflyCamera(cameraConfig,
                                         *butterflyTarget);
#else
    throw ::invalid_argument()
      << info_what("This program don't was compiled with support "
                   "to Butterfly targets");
#endif
  }
  else
  {
      throw ::invalid_argument() << info_what("Unknown target type");
  }

  return abstractCamera;
}
