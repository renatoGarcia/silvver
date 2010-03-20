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
#include "../iplImageWrapper.hpp"
#include "../log.hpp"

namespace bfs = boost::filesystem;
namespace bpt = boost::posix_time;

PseudoCamera::PseudoCamera(const scene::PseudoCamera& config)
  :HardCamera(config, IPL_DEPTH_8U)
  ,path(config.imagesPath)
  ,frameRate(config.frameRate)
  ,dirIterator()
  ,endIterator()
  ,delay((static_cast<long>((1.0 / this->frameRate) * 1.0e3)))
  ,grabFrameThread()
{
  if(!bfs::is_directory(this->path))
  {
    throw open_camera_error()
      << info_what(this->path.directory_string() + " is not a directory");
  }

  this->dirIterator = bfs::directory_iterator(this->path);

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
  bool imageLoaded = false;

  boost::shared_ptr<IplImageWrapper> frameBuffer[2];
  for (int i = 0; i < 2; ++i)
  {
    frameBuffer[i].reset(new IplImageWrapper());
  }

  while (true)
  {
    boost::this_thread::interruption_point();

    imageLoaded = false;
    do
    {
      if(this->dirIterator == this->endIterator)
      {
        message(LogLevel::INFO)
          << ts_output::lock
          << "PseudoCamera already readed all images in directory "
          << this->path.file_string() << std::endl
          << ts_output::unlock;

        return;
      }

      if(bfs::is_directory(dirIterator->status()))
      {
        this->dirIterator++;
        continue;
      }

      try
      {
        frameBuffer[frameIdx]->loadImage(dirIterator->path().file_string(),
                                         CV_LOAD_IMAGE_COLOR);
      }
      catch (load_file_error& e)
      {
        message(LogLevel::WARN)
          << ts_output::lock
          << "PseudoCamera could not load file "
          << this->dirIterator->path().file_string() << std::endl
          << ts_output::unlock;

        this->dirIterator++;
        continue;
      }

      imageLoaded = true;
      this->dirIterator++;
    }while(!imageLoaded);

    updateCurrentFrame(frameBuffer[frameIdx]);

    frameIdx = (frameIdx+1) % 2;

    boost::this_thread::sleep(this->delay);
  }
}
