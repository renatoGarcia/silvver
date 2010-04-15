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

#include "pseudoCamera.hpp"

#include "../exceptions.hpp"
#include "../frame.hpp"
#include "../log.hpp"

namespace bpt = boost::posix_time;

PseudoCamera::PseudoCamera(const scene::PseudoCamera& config)
  :HardCamera(config, IPL_DEPTH_8U)
  ,imagesPath(config.imagesPath)
  ,delay((static_cast<long>((1.0 / config.frameRate) * 1.0e3)))
  ,grabFrameThread()
{
  this->grabFrameThread.reset(new boost::thread(&PseudoCamera::doWork, this));
}

PseudoCamera::~PseudoCamera()
{
  if (this->grabFrameThread)
  {
    this->grabFrameThread->interrupt();
    this->grabFrameThread->join();
  }
}

void
PseudoCamera::doWork()
{
  int frameIdx = 0;

  boost::shared_ptr<Frame> frameBuffer[2];
  for (int i = 0; i < 2; ++i)
  {
    frameBuffer[i].reset(new Frame());
  }

  std::vector<std::string>::const_iterator itImagesPath;
  for (itImagesPath = this->imagesPath.begin();
       itImagesPath < this->imagesPath.end();
       ++itImagesPath)
  {
    boost::this_thread::interruption_point();

    try
    {
      frameBuffer[frameIdx]->loadImage(*itImagesPath, CV_LOAD_IMAGE_COLOR);
      updateCurrentFrame(frameBuffer[frameIdx]);
      frameIdx = (frameIdx+1) % 2;
      boost::this_thread::sleep(this->delay);
    }
    catch (load_file_error& e)
    {
      message(LogLevel::WARN)
        << ts_output::lock
        << "PseudoCamera could not load file " << *itImagesPath << std::endl
        << ts_output::unlock;
      continue;
    }
  }

  message(LogLevel::INFO)
    << ts_output::lock
    << "PseudoCamera already readed all images" << std::endl
    << ts_output::unlock;
}
