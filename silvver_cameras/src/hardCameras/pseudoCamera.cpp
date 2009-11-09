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

#include <boost/bind.hpp>

namespace bfs = boost::filesystem;
namespace bpt = boost::posix_time;

PseudoCamera::PseudoCamera(const scene::PseudoCamera& config)
  :HardCamera(config, PseudoCamera::BITS_PER_PIXEL)
  ,path(config.imagesPath)
  ,frameRate(config.frameRate)
  ,delay((static_cast<long>((1.0 / this->frameRate) * 1.0e3)))
  ,currentFrame(NULL)
  ,frameBuffer({NULL, NULL})
{}

PseudoCamera::~PseudoCamera()
{
  for (int i=0; i<2; ++i)
  {
    if (this->frameBuffer[i] != NULL)
    {
      cvReleaseImage(&this->frameBuffer[i]);
    }
  }
}

void
PseudoCamera::initialize()
{
  if(!bfs::is_directory(this->path))
  {
    throw camera_parameter_error(this->path.directory_string() +
                                 " is not a directory");
  }

  this->dirIterator = bfs::directory_iterator(this->path);

  this->grabFrameThread.reset(new boost::thread(boost::bind<void>
                                                (&PseudoCamera::runCapturer,
                                                 this)));
}

void
PseudoCamera::runCapturer()
{
  int frameIdx = 0;
  bool imageLoaded = false;

  while (true)
  {
    boost::this_thread::interruption_point();

    // Release the buffer
    if (this->frameBuffer[frameIdx] != NULL)
    {
      cvReleaseImage(&this->frameBuffer[frameIdx]);
    }

    imageLoaded = false;
    do
    {
      if(this->dirIterator == this->endIterator)
      {
        // boost::this_thread::sleep(this->delay);
        // throw capture_image_error("PseudoCamera: all images already read");
        // continue;
        return;
      }

      if(bfs::is_directory(dirIterator->status()))
      {
        this->dirIterator++;
        continue;
      }

      this->frameBuffer[frameIdx] =
        cvLoadImage(dirIterator->path().file_string().c_str(),
                    CV_LOAD_IMAGE_COLOR);
      if(this->frameBuffer[frameIdx] == NULL)
      {
        this->dirIterator++;
        continue;
        // throw capture_image_error("Error loading image: " +
        //                           dirIterator->path().file_string());
      }

      imageLoaded = true;
      this->dirIterator++;
    }while(!imageLoaded);

    this->bufferAccess.lock();
    this->currentFrame = this->frameBuffer[frameIdx];
    this->bufferAccess.unlock();

    std::fill(this->unreadImage.begin(), this->unreadImage.end(), true);
    this->unreadFrameCondition.notify_all();

    frameIdx = (frameIdx+1) % 2;

    boost::this_thread::sleep(this->delay);
  }
}

void
PseudoCamera::saveFrame()
{}

void
PseudoCamera::captureFrame(IplImage** iplImage, unsigned clientUid)
{
  boost::shared_lock<boost::shared_mutex> lock(this->bufferAccess);

  while (!this->unreadImage.at(clientUid))
  {
    this->unreadFrameCondition.wait(lock);
  }

  cvReleaseImage(iplImage);
  *iplImage = cvCloneImage(this->currentFrame);

  this->unreadImage.at(clientUid) = false;
}
