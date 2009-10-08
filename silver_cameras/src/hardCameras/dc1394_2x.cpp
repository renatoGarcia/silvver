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

#include "dc1394_2x.hpp"

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/ref.hpp>
#include <boost/bind.hpp>

DC1394::DC1394(const scene::DC1394& config)
  :HardCamera(config)
  ,uid(config.uid)
  ,frameRate(config.frameRate)
  ,context(NULL)
  ,camera(NULL)
  ,currentFrame(NULL)
{
  for (int i = 0; i < 2; ++i)
  {
    this->frameBuffer[i] = NULL;
  }
  this->context = dc1394_new();
  if (!this->context)
  {
    throw open_camera_error("Unable to create a libdc1394 context");
  }
}

DC1394::~DC1394()
{
  if (this->grabFrameThread)
  {
    this->grabFrameThread->interrupt();
    this->grabFrameThread->join();
  }

  for (int i = 0; i < 2; ++i)
  {
    if (this->frameBuffer[i] != NULL)
    {
      // Release the buffer
      dc1394_capture_enqueue(this->camera, this->frameBuffer[i]);
    }
  }

  if (this->camera)
  {
    dc1394_video_set_transmission(this->camera, DC1394_OFF);
    dc1394_capture_stop(this->camera);
    dc1394_camera_free(this->camera);
  }

  dc1394_free(this->context);
}

dc1394video_mode_t
DC1394::getDc1394VideoMode() const
{
  if (this->frameWidth == 640 && this->frameHeight == 480)
  {
    return DC1394_VIDEO_MODE_640x480_MONO8;
  }
  else if (this->frameWidth == 800 && this->frameHeight == 600)
  {
    return DC1394_VIDEO_MODE_800x600_MONO8;
  }
  else if (this->frameWidth == 1024 && this->frameHeight == 768)
  {
    return DC1394_VIDEO_MODE_1024x768_MONO8;
  }
  else if (this->frameWidth == 1280 && this->frameHeight == 960)
  {
    return DC1394_VIDEO_MODE_1280x960_MONO8;
  }
  else if (this->frameWidth == 1600 && this->frameHeight == 1200)
  {
    return DC1394_VIDEO_MODE_1600x1200_MONO8;
  }
  else
  {
    throw open_camera_error("Invalid resolution " +
                            boost::lexical_cast<std::string>(this->frameWidth) +
                            " x " +
                            boost::lexical_cast<std::string>(this->frameHeight));
  }
}

dc1394framerate_t
DC1394::getDc1394FrameRate() const
{
  if (this->frameRate == 1.875)
  {
    return DC1394_FRAMERATE_1_875;
  }
  else if (this->frameRate == 3.75)
  {
    return DC1394_FRAMERATE_3_75;
  }
  else if (this->frameRate == 7.5)
  {
    return DC1394_FRAMERATE_7_5;
  }
  else if (this->frameRate == 15.0)
  {
    return DC1394_FRAMERATE_15;
  }
  else if (this->frameRate == 30.0)
  {
    return DC1394_FRAMERATE_30;
  }
  else if (this->frameRate == 60.0)
  {
    return DC1394_FRAMERATE_60;
  }
  else if (this->frameRate == 120.0)
  {
    return DC1394_FRAMERATE_120;
  }
  else if (this->frameRate == 240.0)
  {
    return DC1394_FRAMERATE_240;
  }
  else
  {
    throw open_camera_error("Invalid frame rate " +
                            boost::lexical_cast<std::string>(this->frameRate));
  }
}

void
DC1394::initialize()
{
  this->camera = dc1394_camera_new(this->context,
                                   boost::lexical_cast<uint64_t>(this->uid));
  if (!this->camera)
  {
    throw open_camera_error("Don't found camera with uid " + this->uid);
  }

  if (dc1394_video_set_iso_speed(this->camera, DC1394_ISO_SPEED_400))
  {
    throw open_camera_error("Could not set 400Mbps iso speed");
  }

  if (dc1394_video_set_mode(camera, this->getDc1394VideoMode()))
  {
    throw open_camera_error("Could not set video mode");
  }

  if (dc1394_video_set_framerate(camera, this->getDc1394FrameRate()))
  {
    throw open_camera_error("Could not set framerate");
  }

  if (dc1394_capture_setup(this->camera, DC1394::N_BUFFERS,
                           DC1394_CAPTURE_FLAGS_DEFAULT))
  {
    throw open_camera_error("Could not setup camera. Make sure that the resolution and framerate are supported by your camera");
  }

  if (dc1394_video_set_transmission(this->camera, DC1394_ON))
  {
    throw open_camera_error("Could not start camera iso transmission");
  }
  this->grabFrameThread.reset(new boost::thread(boost::bind<void>
                                                (&DC1394::runCapturer,
                                                 this)));
}

void
DC1394::runCapturer()
{
  int frameIdx = 0;

  // Capture the first two frames to initialize the frameBuffer array.
  for(int i = 0; i < 2; ++i)
  {
    if (dc1394_capture_dequeue(this->camera,
                               DC1394_CAPTURE_POLICY_WAIT,
                               &this->frameBuffer[i]))
    {
      throw capture_image_error("Could not capture a frame");
    }
  }
  while (true)
  {
    boost::this_thread::interruption_point();

    // Release the buffer
    if (dc1394_capture_enqueue(this->camera, this->frameBuffer[frameIdx]))
    {
      throw capture_image_error("Error on returning a frame to ring buffer");
    }

    if (dc1394_capture_dequeue(this->camera,
                               DC1394_CAPTURE_POLICY_WAIT,
                               &this->frameBuffer[frameIdx]))
    {
      throw capture_image_error("Could not capture a frame");
    }

    this->bufferAccess.lock();
    this->currentFrame = this->frameBuffer[frameIdx];
    this->bufferAccess.unlock();

    std::fill(this->unreadImage.begin(), this->unreadImage.end(), true);
    this->unreadFrameCondition.notify_all();

    frameIdx = (frameIdx+1) % 2;
  }
}

void
DC1394::captureFrame(IplImage** iplImage, unsigned clientUid)
{
  boost::shared_lock<boost::shared_mutex> lock(this->bufferAccess);

  while (!this->unreadImage.at(clientUid))
  {
    this->unreadFrameCondition.wait(lock);
  }

  // This don't work for images whith more than 1 byte per pixel
  if (this->currentFrame->color_coding == DC1394_COLOR_CODING_MONO8)
  {
    if (dc1394_bayer_decoding_8bit(this->currentFrame->image,
                                   (uint8_t*)(*iplImage)->imageData,
                                   this->frameWidth,
                                   this->frameHeight,
                                   DC1394_COLOR_FILTER_RGGB,
                                   DC1394_BAYER_METHOD_SIMPLE))
    {
      throw capture_image_error("Error on bayer decoding");

    }
  }

  this->unreadImage.at(clientUid) = false;
}
