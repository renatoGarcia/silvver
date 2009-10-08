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

#include "dc1394_1x.hpp"

#include <algorithm>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <cstddef>
#include <cstring>

#include <sys/stat.h>

DC1394::DC1394(const scene::Camera& config,
               Format format)
  :HardCamera(config)
  ,raw1394Handle(NULL)
  ,bDc1394CameraCreated(false)
{
  // if mode == MODE_640x40_MONO
  this->bytesPerPixel = 1;

  for (int i = 0; i < 2; ++i)
  {
    this->frameBuffer[i] = new unsigned char[this->frameSize *
                                             this->bytesPerPixel];
  }

  this->mode = MODE_640x480_MONO;
  this->format = format;
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
    delete[] this->frameBuffer[i];
  }

  dc1394_stop_iso_transmission(this->raw1394Handle, this->dc1394Camera.node);

  if(this->bDc1394CameraCreated)
  {
    dc1394_dma_unlisten(this->raw1394Handle, &(this->dc1394Camera));
    dc1394_dma_release_camera(this->raw1394Handle, &(this->dc1394Camera));
  }

  if(this->raw1394Handle)
  {
    raw1394_destroy_handle(this->raw1394Handle);
  }
}

void
DC1394::findThisCamera(nodeid_t& node, int& index)
{
  raw1394handle_t tmpHandle;
  tmpHandle = raw1394_new_handle();
  int nCards = raw1394_get_port_info(tmpHandle, NULL, 0);
  raw1394_destroy_handle(tmpHandle);

  int	     nCameras;
  nodeid_t*  cameraNodes;
  dc1394_camerainfo cameraInfo;
  for(int iCard = 0; iCard < nCards; ++iCard)
  {
    this->raw1394Handle = dc1394_create_handle(iCard);
    if(this->raw1394Handle == NULL)
    {
      throw open_camera_error("Unable to aquire a raw1394 handle");
    }

    cameraNodes = dc1394_get_camera_nodes(this->raw1394Handle, &nCameras, 0);

    // Find the camera with this->uid
    for(int cameraIndex=0; cameraIndex < nCameras; cameraIndex++)
    {
      dc1394_get_camera_info(this->raw1394Handle, cameraNodes[cameraIndex],
                             &cameraInfo);
      if(boost::lexical_cast<std::string>(cameraInfo.euid_64) == this->uid)
      {
        // Return values
        node  = cameraNodes[cameraIndex];
        index = cameraIndex;
        return;
      }
    }

    raw1394_destroy_handle(this->raw1394Handle);
    this->raw1394Handle = NULL;
  }

  throw open_camera_error("Don't found a camera with uid " + this->uid);
}

std::string
DC1394::findVideo1394Device(unsigned nodeNumber)
{
  std::string strNodeNumber(boost::lexical_cast<std::string>(nodeNumber));

  boost::array<std::string, 3> possibleDevices;
  possibleDevices.at(0) = "/dev/video1394/" + strNodeNumber;
  possibleDevices.at(1) = "/dev/video1394-" + strNodeNumber;
  possibleDevices.at(2) = "/dev/video1394" + strNodeNumber;

  struct stat statbuf;
  std::string device;
  BOOST_FOREACH (device, possibleDevices)
  {
    if (stat(device.c_str(), &statbuf) == 0 && S_ISCHR(statbuf.st_mode))
    {
      return device;
    }
  }

  // If here, don't found camera devide
  throw open_camera_error("Don't found the device of camera with uid " +
                          this->uid);
}

int
DC1394::getDc1394FrameRate()
{
  if (this->frameRate == 1.875)
  {
    return FRAMERATE_1_875;
  }
  else if (this->frameRate == 3.75)
  {
    return FRAMERATE_3_75;
  }
  else if (this->frameRate == 7.5)
  {
    return FRAMERATE_7_5;
  }
  else if (this->frameRate == 15.0)
  {
    return FRAMERATE_15;
  }
  else if (this->frameRate == 30.0)
  {
    return FRAMERATE_30;
  }
  else if (this->frameRate == 60.0)
  {
    return FRAMERATE_60;
  }
  else if (this->frameRate == 120.0)
  {
    return FRAMERATE_120;
  }
  else if (this->frameRate == 240.0)
  {
    return FRAMERATE_240;
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
  nodeid_t cameraNode;
  int cameraIndex;
  findThisCamera(cameraNode, cameraIndex);

  int dc1394FrameRate = this->getDc1394FrameRate();

  unsigned int channel;
  unsigned int speed;
  if (dc1394_get_iso_channel_and_speed(this->raw1394Handle,
                                       cameraNode,
                                       &channel,
                                       &speed ) !=DC1394_SUCCESS)
  {
    throw open_camera_error("Unable to get the iso channel number");
  }

  int e = dc1394_dma_setup_capture(this->raw1394Handle,
                                   cameraNode,
                                   cameraIndex,
                                   this->format,
                                   this->mode,
                                   speed,
                                   dc1394FrameRate,
                                   8,	// number of buffers
                                   1,	// drop frames
                                   this->findVideo1394Device(cameraNode).c_str(),
                                   &(this->dc1394Camera));
  if (e != DC1394_SUCCESS)
  {
    throw open_camera_error("Unable to setup camera");
  }
  else
  {
    this->bDc1394CameraCreated = true;
  }


  /*-----------------------------------------------------------------------
   *  have the camera start sending us data
   *-----------------------------------------------------------------------*/
  if (dc1394_start_iso_transmission(this->raw1394Handle,
                                    this->dc1394Camera.node)
      != DC1394_SUCCESS)
  {
    throw open_camera_error("Unable to start camera iso transmission");
  }


  /*-----------------------------------------------------------------------
   *  query the camera to determine the Bayer pattern
   *-----------------------------------------------------------------------*/
  quadlet_t qValue;
  GetCameraControlRegister(this->raw1394Handle,
                           cameraNode,
                           0x1040,         /* Bayer Tile Mapping register */
                           &qValue );

  switch(qValue)
  {
  case 0x42474752:      /* BGGR */
    this->pattern = BAYER_PATTERN_BGGR;
    break;
  case 0x47524247:      /* GRBG */
    this->pattern = BAYER_PATTERN_GRBG;
    break;
  case 0x52474742:      /* RGGB */
    this->pattern = BAYER_PATTERN_RGGB;
    break;
  case 0x47425247:      /* GBRG */
    this->pattern = BAYER_PATTERN_GBRG;
    break;
  case 0x59595959:      /* YYYY = BW */
    throw open_camera_error("Camera is black and white");
  default:
    throw open_camera_error("Camera BAYER_TILE_MAPPING register has an unexpected value");
  }

  this->grabFrameThread.reset(new boost::thread(boost::bind<void>
                                                (&DC1394::runCapturer,
                                                 this)));
}

void
DC1394::runCapturer()
{
  int frameIdx = 0;

  while (true)
  {
    boost::this_thread::interruption_point();

    if (dc1394_dma_single_capture(&(this->dc1394Camera)) != DC1394_SUCCESS)
    {
      throw capture_image_error("Unable to capture a frame");
    }

    memcpy(this->frameBuffer[frameIdx],
           this->dc1394Camera.capture_buffer,
           this->frameSize * this->bytesPerPixel);

    this->bufferAccess.lock();
    this->currentFrame = this->frameBuffer[frameIdx];
    this->bufferAccess.unlock();

    std::fill(this->unreadImage.begin(), this->unreadImage.end(), true);
    this->unreadFrameCondition.notify_all();

    dc1394_dma_done_with_buffer(&(this->dc1394Camera));

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

  BayerEdgeSense(this->currentFrame,
                 (unsigned char*)(*iplImage)->imageData,
                 this->dc1394Camera.frame_width,
                 this->dc1394Camera.frame_height,
                 this->pattern);

  this->unreadImage.at(clientUid) = false;
}
