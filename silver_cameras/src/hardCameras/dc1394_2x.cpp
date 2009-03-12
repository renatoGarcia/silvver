#include <iostream>
using namespace std;

#include "dc1394_2x.hpp"
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>

DC1394::DC1394(int nCard, uint64 UID, FrameRate frameRate,
               Resolution resolution, Format format)
  :HardCamera(UID, resolution, frameRate)
{
  this->nCard = nCard;
  this->device = "/dev/video1394/" + boost::lexical_cast<std::string>(nCard);

  this->mode = MODE_640x480_MONO;
  this->format = format;

  // if mode == MODE_640x40_MONO
  this->bytesPerPixel = 1;

  this->bRaw1394HandleCreated = false;
  this->bDc1394CameraCreated = false;
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
  this->raw1394Handle = dc1394_create_handle( this->nCard );
  if(this->raw1394Handle == NULL)
  {
    throw OpenCameraFailed("Unable to aquire a raw1394 handle\n\n"
                           "Please check\n"
                           "  - if the kernel modules `ieee1394',`raw1394' and `ohci1394' are loaded\n"
                           "  - if you have read/write access to /dev/raw1394\n");
  }
  this->bRaw1394HandleCreated = true;

  int numNodes;
  numNodes = raw1394_get_nodecount( this->raw1394Handle );

  nodeid_t* cameraNodes;
  int	    numCameras;
  /* Note: set 3rd parameter to 0 if you do not want the camera details printed */
  cameraNodes = dc1394_get_camera_nodes(this->raw1394Handle, &numCameras, 0);
  if(numCameras < 1)
  {
    throw OpenCameraFailed("No cameras found");
  }

  /*-----------------------------------------------------------------------
   *  to prevent the iso-transfer bug from raw1394 system, check if
   *  camera is highest node. For details see
   *  http://linux1394.sourceforge.net/faq.html#DCbusmgmt
   *  and
   *  http://sourceforge.net/tracker/index.php?func=detail&aid=435107&group_id=8157&atid=108157
   *-----------------------------------------------------------------------*/
  if(cameraNodes[0] == numNodes-1)
  {
    std::cerr << "\nSorry, your camera is the highest numbered node\n"
              << "of the bus, and has therefore become the root node.\n"
              << "The root node is responsible for maintaining \n"
              << "the timing of isochronous transactions on the IEEE \n"
              << "1394 bus.  However, if the root node is not cycle master \n"
              << "capable (it doesn't have to be), then isochronous \n"
              << "transactions will not work.  The host controller card is \n"
              << "cycle master capable, however, most cameras are not.\n"
              << "\n"
              << "The quick solution is to add the parameter \n"
              << "attempt_root=1 when loading the OHCI driver as a \n"
              << "module.  So please do (as root):\n"
              << "\n"
              << "   rmmod ohci1394\n"
              << "   insmod ohci1394 attempt_root=1\n"
              << "\n"
              << "for more information see the FAQ at \n"
              << "http://linux1394.sourceforge.net/faq.html#DCbusmgmt\n"
              << std::endl;
    throw OpenCameraFailed("Your camera is the highest numbered node");
  }

  /*-----------------------------------------------------------------------
   *  Find the camera with this->uid
   *-----------------------------------------------------------------------*/
  int cameraIndex;
  dc1394_camerainfo cameraInfo;
  for(cameraIndex=0; cameraIndex < numCameras; cameraIndex++)
  {
    dc1394_get_camera_info(this->raw1394Handle, cameraNodes[cameraIndex],
                           &cameraInfo);
    if(cameraInfo.euid_64 == this->UID)
    {
      break;
    }
  }
  if(cameraIndex == numCameras)
  {
    throw std::invalid_argument("It don't find a camera with this UID");
  }

  /*-----------------------------------------------------------------------
   *  setup capture
   *-----------------------------------------------------------------------*/
  int dc1394FrameRate;
  switch(this->frameRate)
  {
  case HardCamera::FR_7_5:
    dc1394FrameRate = FRAMERATE_7_5;
    break;

  case HardCamera::FR_15:
    dc1394FrameRate = FRAMERATE_15;
    break;

  case HardCamera::FR_30:
    dc1394FrameRate = FRAMERATE_30;
    break;

  default:
    throw OpenCameraFailed("Frame Rate don't allowed");

  }

  unsigned int channel;
  unsigned int speed;
  if ( dc1394_get_iso_channel_and_speed( this->raw1394Handle,
                                         cameraNodes[cameraIndex],
                                         &channel,
                                         &speed ) !=DC1394_SUCCESS )
  {
    throw OpenCameraFailed("Unable to get the iso channel number");
  }

  // note: format, mode, frameRate and bytesPerPixel are all defined as globals 
  // in the header
  int e = dc1394_dma_setup_capture( this->raw1394Handle,
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
  if ( e != DC1394_SUCCESS )
  {
    throw OpenCameraFailed("Unable to setup camera");
  }
  this->bDc1394CameraCreated = true;


  /*-----------------------------------------------------------------------
   *  have the camera start sending us data
   *-----------------------------------------------------------------------*/
  if ( dc1394_start_iso_transmission( this->raw1394Handle,
                                      this->dc1394Camera.node)
       != DC1394_SUCCESS )
  {
    throw OpenCameraFailed("Unable to start camera iso transmission");
  }


  /*-----------------------------------------------------------------------
   *  query the camera to determine the Bayer pattern
   *-----------------------------------------------------------------------*/
  quadlet_t qValue;
  GetCameraControlRegister( this->raw1394Handle,
                            cameraNodes[cameraIndex],
                            0x1040,         /* Bayer Tile Mapping register */
                            &qValue );

  switch( qValue )
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
    throw OpenCameraFailed("Camera is black and white");
  default:
    throw OpenCameraFailed("Camera BAYER_TILE_MAPPING register has an unexpected value");
  }

  return;
}

void
DC1394::captureFrame(IplImage* &iplImage)
{
  boost::mutex::scoped_lock lock(this->mutexCaptureFrame);

  // Capture one frame
  if (dc1394_dma_single_capture(&(this->dc1394Camera)) != DC1394_SUCCESS)
  {
    throw CaptureImageError("Unable to capture a frame");
  }

  unsigned char* src = (unsigned char*) this->dc1394Camera.capture_buffer;
  if(this->bytesPerPixel > 1)
  {
    src = new unsigned char[this->frameSize];
    if(!src)
    {
      throw CaptureImageError("Could not allocate copy buffer for color conversion");
    }
    unsigned char* captureBuffer =
      (unsigned char*) this->dc1394Camera.capture_buffer;
    for(unsigned i = 0; i < this->frameSize; i++)
    {
      src[i] = captureBuffer[i * this->bytesPerPixel];
    }
  }

  BayerEdgeSense(src,
                 (unsigned char*)iplImage->imageData,
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
    throw CaptureImageError("Unable to capture a frame");
  }

  unsigned char* rgbBuffer = new unsigned char[3 * frameSize];
  if(!rgbBuffer)
  {
    throw CaptureImageError("Could not allocate RGB buffer for color conversion");
  }

  unsigned char* src = (unsigned char*) this->dc1394Camera.capture_buffer;
  if(this->bytesPerPixel > 1)
  {
    src = new unsigned char[this->frameSize];
    if(!src)
    {
    throw CaptureImageError("Could not allocate copy buffer for color conversion");
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
