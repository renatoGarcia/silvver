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

#include "exceptions.hpp"
#include "log.hpp"

#include "hardCameras/pseudoCamera.hpp"

#ifdef HAVE_PGRFLYCAPTURE_HEADERS
#  include "hardCameras/pgr.hpp"
#endif

#ifdef HAS_DC1394V1
#  include "hardCameras/dc1394_1x.hpp"
#  define HAS_DC1394
#endif
#ifdef HAS_DC1394V2
#  include "hardCameras/dc1394_2x.hpp"
#  define HAS_DC1394
#endif

#ifdef HAS_V4L2
#  include "hardCameras/v4l2.hpp"
#endif

std::map < std::string, boost::shared_ptr<HardCamera> >
HardCameraFactory::createdHardCameras;

boost::mutex
HardCameraFactory::mutexCameraCreate;

boost::shared_ptr<HardCamera>
HardCameraFactory::create(const scene::AnyHardwareCamera& cameraConfig)
{
  // HardCameras can't be simultaneously initialized
  boost::mutex::scoped_lock lock(HardCameraFactory::mutexCameraCreate);

  boost::shared_ptr<HardCamera> hardCamera;

  const scene::Hardware hardwareConfig =
    boost::apply_visitor(scene::GetHardware(), cameraConfig);

  std::map< std::string, boost::shared_ptr<HardCamera> >::iterator
    iteHardCamera = HardCameraFactory::createdHardCameras.find(hardwareConfig.identifier);

  // If hardCamera is already created
  if (iteHardCamera != HardCameraFactory::createdHardCameras.end())
  {
    hardCamera = iteHardCamera->second;
  }
  else
  {
    hardCamera.reset(boost::apply_visitor
                     (HardCameraFactory::ConstructHardCamera(),
                      cameraConfig));

    HardCameraFactory::createdHardCameras.
      insert(std::pair< std::string, boost::shared_ptr<HardCamera> >
             (hardwareConfig.identifier, hardCamera));
  }

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
#ifdef HAS_DC1394
  try
  {
    message(LogLevel::TRACE)
      << "HardCamera factory building DC1394 camera uid "
      << config.uid << std::endl;
    return (new DC1394(config));
  }
  catch(silvver_cameras_exception& e)
  {
    throw e << info_cameraModel("DC1394");
  }
#else
  throw invalid_argument()
    << info_what("This program don't was compiled with support "
                 "to ieee 1394 cameras");
#endif
}

HardCamera*
HardCameraFactory::ConstructHardCamera::operator()(const scene::V4l2& config) const
{
#ifdef HAS_V4L2
  try
  {
    message(LogLevel::TRACE)
      << "HardCamera factory building V4L2 camera uid "
      << config.uid << std::endl;
    return (new V4L2(config));
  }
  catch(silvver_cameras_exception& e)
  {
    throw e << info_cameraModel("V4l2");
  }
#else
  throw invalid_argument()
    << info_what("This program don't was compiled with support"
                 "to v4l2 cameras");
#endif
}
