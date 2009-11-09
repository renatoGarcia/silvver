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

#include "hardCameraFactory.hpp"

#include <stdexcept>

#include "hardCameras/pseudoCamera.hpp"
#ifdef HAVE_PGRFLYCAPTURE_HEADERS
 #include "hardCameras/pgr.hpp"
#endif
#ifdef HAVE_LIBDC1394_DC1394_CONTROL_H
 #include "hardCameras/dc1394_1x.hpp"
 #define HAVE_DC1394
#endif
#ifdef HAVE_DC1394_DC1394_H
 #include "hardCameras/dc1394_2x.hpp"
 #define HAVE_DC1394
#endif

std::map < std::string, boost::shared_ptr<HardCamera> >
HardCameraFactory::createdHardCameras;

boost::mutex
HardCameraFactory::mutexCameraCreate;

boost::tuple<boost::shared_ptr<HardCamera>, unsigned>
HardCameraFactory::create(const scene::AnyHardwareCamera& cameraConfig)
{
  // HardCameras can't be simultaneously initialized
  boost::mutex::scoped_lock lock(HardCameraFactory::mutexCameraCreate);

  boost::tuple<boost::shared_ptr<HardCamera>, unsigned> hardCamera;

  const scene::Hardware hardwareConfig =
    boost::apply_visitor(scene::GetHardware(), cameraConfig);

  std::map< std::string, boost::shared_ptr<HardCamera> >::iterator
    iteHardCamera = HardCameraFactory::createdHardCameras.find(hardwareConfig.uid);

  // If hardCamera is already created
  if(iteHardCamera != HardCameraFactory::createdHardCameras.end())
  {
    hardCamera.get<0>() = iteHardCamera->second;
  }
  else
  {

    hardCamera.get<0>().reset(boost::apply_visitor
                              (HardCameraFactory::ConstructHardCamera(),
                               cameraConfig));

    hardCamera.get<0>()->initialize();

    HardCameraFactory::createdHardCameras.
      insert(std::pair< std::string, boost::shared_ptr<HardCamera> >
             (hardwareConfig.uid, hardCamera.get<0>()));
  }

  hardCamera.get<1>() = hardCamera.get<0>()->addClient();

  return hardCamera;
}

HardCamera*
HardCameraFactory::ConstructHardCamera::operator()(const scene::PseudoCamera& config) const
{
  return (new PseudoCamera(config));
}

HardCamera*
HardCameraFactory::ConstructHardCamera::operator()(const scene::DC1394& config) const
{
#ifdef HAVE_DC1394
  return (new DC1394(config));
#else
  throw std::invalid_argument("This program don't was compiled with support "\
                              "to ieee 1394 cameras");
#endif
}
