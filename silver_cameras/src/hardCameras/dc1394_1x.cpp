#include <iostream>
using namespace std;

#include "dc1394_1x.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>

DC1394::DC1394(int nCard,
               const scene::Camera& config,
               Format format)
  :HardCamera(config)
  ,nCard(nCard)
  ,device("/dev/video1394-0")
//   ,device("/dev/video1394/" + boost::lexical_cast<std::string>(nCard))
  ,bRaw1394HandleCreated(false)
  ,bDc1394CameraCreated(false)
{
  this->mode = MODE_640x480_MONO;
  this->format = format;

  // if mode == MODE_640x40_MONO
  this->bytesPerPixel = 1;
}

DC1394::~DC1394()
{
  dc1394_stop_iso_transmission(this->raw1394Handle, this->dc1394Camera.node);

  if(this->bDc1394CameraCreated)
  {
    dc1394_dma_unlisten(this->raw1394Handle, &(this->dc1394Camera));
    dc1394_dma_release_camera(this->raw1394Handle, &(this->dc1394Camera));
  }

  if(this->bRaw1394HandleCreated)
  {
    raw1394_destroy_handle(this->raw1394Handle);
  }
}

void
DC1394::initialize()
{
  this->raw1394Handle = dc1394_create_handle(this->nCard);
  if(this->raw1394Handle == NULL)
  {
    throw open_camera_error("Unable to aquire a raw1394 handle");
  }
  else
  {
    this->bRaw1394HandleCreated = true;
  }

  int numNodes;
  numNodes = raw1394_get_nodecount(this->raw1394Handle);

  nodeid_t* cameraNodes;
  int	    nCameras;
  cameraNodes = dc1394_get_camera_nodes(this->raw1394Handle, &nCameras, 0);
  if(nCameras < 1)
  {
    throw open_camera_error("No cameras found");
  }

  // To prevent the iso-transfer bug from raw1394 system, check if
  // camera is highest node. For details see
  // http://linux1394.sourceforge.net/faq.html#DCbusmgmt
  // and
  // http://sourceforge.net/tracker/index.php?func=detail&aid=435107&group_id=8157&atid=108157
  // The quick solution is to add the parameter attempt_root=1 when loading
  // the OHCI driver as a module.
  if(cameraNodes[0] == numNodes-1)
  {
    throw open_camera_error("Your camera is the highest numbered node");
  }

  /*-----------------------------------------------------------------------
   *  Find the camera with this->uid
   *-----------------------------------------------------------------------*/
  int cameraIndex;
  dc1394_camerainfo cameraInfo;
  for(cameraIndex=0; cameraIndex < nCameras; cameraIndex++)
  {
    dc1394_get_camera_info(this->raw1394Handle, cameraNodes[cameraIndex],
                           &cameraInfo);
    if(boost::lexical_cast<std::string>(cameraInfo.euid_64) == this->uid)
    {
      break;
    }
  }
  if(cameraIndex == nCameras)
  {
    throw open_camera_error("Don't found a camera with uid " + this->uid);
  }

  /*-----------------------------------------------------------------------
   *  setup capture
   *-----------------------------------------------------------------------*/
  int dc1394FrameRate;
  if (this->frameRate == 7.5)
  {
    dc1394FrameRate = FRAMERATE_7_5;
  }
  else if (this->frameRate == 15.0)
  {
    dc1394FrameRate = FRAMERATE_15;
  }
  else if (this->frameRate == 30.0)
  {
    dc1394FrameRate = FRAMERATE_30;
  }
  else
  {
    throw open_camera_error("Frame Rate don't allowed");
  }

  unsigned int channel;
  unsigned int speed;
  if (dc1394_get_iso_channel_and_speed(this->raw1394Handle,
                                       cameraNodes[cameraIndex],
                                       &channel,
                                       &speed ) !=DC1394_SUCCESS)
  {
    throw open_camera_error("Unable to get the iso channel number");
  }

  // Note: format, mode, frameRate and bytesPerPixel are all defined as globals
  // in the header of dc1394 library
  int e = dc1394_dma_setup_capture(this->raw1394Handle,
                                   cameraNodes[cameraIndex],
                                   cameraIndex,
                                   this->format,
                                   this->mode,
                                   speed,
                                   dc1394FrameRate,
                                   8,	// number of buffers
                                   1,	// drop frames
                                   this->device.c_str(),
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
                           cameraNodes[cameraIndex],
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

  return;
}

void
DC1394::captureFrame(IplImage** iplImage)
{
  boost::mutex::scoped_lock lock(this->mutexCaptureFrame);

  // Capture one frame
  if (dc1394_dma_single_capture(&(this->dc1394Camera)) != DC1394_SUCCESS)
  {
    throw capture_image_error("Unable to capture a frame");
  }

  unsigned char* src = (unsigned char*) this->dc1394Camera.capture_buffer;
  if(this->bytesPerPixel > 1)
  {
    src = new unsigned char[this->frameSize];
    if(!src)
    {
      throw capture_image_error("Could not allocate copy buffer for color conversion");
    }
    unsigned char* captureBuffer =
      (unsigned char*) this->dc1394Camera.capture_buffer;
    for(unsigned i = 0; i < this->frameSize; i++)
    {
      src[i] = captureBuffer[i * this->bytesPerPixel];
    }
  }

  BayerEdgeSense(src,
                 (unsigned char*)(*iplImage)->imageData,
                 this->dc1394Camera.frame_width,
                 this->dc1394Camera.frame_height,
                 this->pattern );

  // Release the buffer
  dc1394_dma_done_with_buffer(&(this->dc1394Camera));

  return;
}

void
DC1394::saveFrame()
{
  /*-----------------------------------------------------------------------
   *  capture one frame to save to file
   *-----------------------------------------------------------------------*/
  if (dc1394_dma_single_capture( &(this->dc1394Camera) ) != DC1394_SUCCESS)
  {
    throw capture_image_error("Unable to capture a frame");
  }

  unsigned char* rgbBuffer = new unsigned char[3 * frameSize];
  if(!rgbBuffer)
  {
    throw capture_image_error("Could not allocate RGB buffer for color conversion");
  }

  unsigned char* src = (unsigned char*) this->dc1394Camera.capture_buffer;
  if(this->bytesPerPixel > 1)
  {
    src = new unsigned char[this->frameSize];
    if(!src)
    {
    throw capture_image_error("Could not allocate copy buffer for color conversion");
    }
    unsigned char* captureBuffer =
      (unsigned char*) this->dc1394Camera.capture_buffer;
    for(unsigned i = 0; i < this->frameSize; i++)
    {
      src[i] = captureBuffer[i * this->bytesPerPixel];
    }
  }

  /*-----------------------------------------------------------------------
   *  convert to color image
   *-----------------------------------------------------------------------*/
  BayerEdgeSense(src,
                 rgbBuffer,
                 this->frameWidth,
                 this->frameHeight,
                 this->pattern);

  /*-----------------------------------------------------------------------
   *  save image
   *-----------------------------------------------------------------------*/
  std::ofstream outfile("image.ppm");

  outfile << "P6" << std::endl
          << this->dc1394Camera.frame_width << " "
          << this->dc1394Camera.frame_height << " "
          << "255" << std::endl;

  outfile.write((const char*)rgbBuffer, this->frameSize * 3);
  outfile.close();

  dc1394_dma_done_with_buffer(&(this->dc1394Camera));

  delete rgbBuffer;
}
