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
#include "processorOptions.hpp"

#ifdef HAS_ARTKP
#  include "abstractCameras/artkpCamera.hpp"
#endif
#ifdef HAS_BORBOLETA
#  include "abstractCameras/borboletaCamera.hpp"
#endif

AbstractCamera*
AbstractCameraFactory::create(const scene::Camera& cameraConfig,
                              const scene::AnyTarget& targets,
                              boost::shared_ptr<Connection> connection)
{
  AbstractCamera* abstractCamera;

  if (const scene::ArtkpTargets* const artkpTarget =
      boost::get<scene::ArtkpTargets>(&targets))
  {
#ifdef HAS_ARTKP
    procOpt::AnyProcOpt markerOpt = procOpt::Marker();
    connection->connect(markerOpt);

    abstractCamera = new ArtkpCamera(cameraConfig,
                                     *artkpTarget,
                                     connection);
#else
    throw invalid_argument()
      << info_what("This program don't was compiled with support "
                   "to ARToolKitPlus targets");
#endif
  }
  if (const scene::BorboletaTargets* const borboletaTarget =
      boost::get<scene::BorboletaTargets>(&targets))
  {
#ifdef HAS_BORBOLETA
    procOpt::AnyProcOpt markerOpt = procOpt::Marker();
    connection->connect(markerOpt);

    abstractCamera = new BorboletaCamera(cameraConfig,
                                         *borboletaTarget,
                                         connection);
#else
    throw invalid_argument()
      << info_what("This program don't was compiled with support "
                   "to Borboleta targets");
#endif
  }
  else
  {
    throw invalid_argument() << info_what("Unknown target type");
  }

  return abstractCamera;
}
