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

#include <boost/assign/std/vector.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <cstddef>
#include <cstring>
#include <stdint.h>

#include <sys/stat.h>

using namespace boost::assign;

DC1394::DC1394(const scene::DC1394& config)
  :HardCamera(config, getIplDepth(config.colorMode),
              getNChannels(config.colorMode))
  ,uid(config.uid)
  ,raw1394Handle(NULL)
  ,bDc1394CameraCreated(false)
  ,videoMode(getDc1394VideoMode(config.colorMode))
  ,grabFrameThread()
{
  nodeid_t cameraNode;
  int cardIndex;
  findThisCamera(cameraNode, cardIndex);

  int dc1394FrameRate = this->getDc1394FrameRate(config.frameRate);

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
                                   cameraNode, // ISO channel. Is will work with multiples firewire cards?
                                   FORMAT_VGA_NONCOMPRESSED,
                                   this->videoMode,
                                   speed,
                                   dc1394FrameRate,
                                   DC1394::N_BUFFERS,
                                   1,	// drop frames
                                   this->findVideo1394Device(cardIndex).c_str(),
                                   &(this->dc1394Camera));
  if (e != DC1394_SUCCESS)
  {
    throw open_camera_error("Unable to setup camera");
  }
  else
  {
    this->bDc1394CameraCreated = true;
  }

  this->setFeatures(config, cameraNode);

  // Have the camera start sending us data
  if (dc1394_start_iso_transmission(this->raw1394Handle,
                                    this->dc1394Camera.node)
      != DC1394_SUCCESS)
  {
    throw open_camera_error("Unable to start camera iso transmission");
  }


  // // Query the camera to determine the Bayer pattern
  // quadlet_t qValue;
  // GetCameraControlRegister(this->raw1394Handle,
  //                          cameraNode,
  //                          0x1040,         /* Bayer Tile Mapping register */
  //                          &qValue );

  // switch(qValue)
  // {
  // case 0x42474752:      /* BGGR */
  //   this->pattern = BAYER_PATTERN_BGGR;
  //   break;
  // case 0x47524247:      /* GRBG */
  //   this->pattern = BAYER_PATTERN_GRBG;
  //   break;
  // case 0x52474742:      /* RGGB */
  //   this->pattern = BAYER_PATTERN_RGGB;
  //   break;
  // case 0x47425247:      /* GBRG */
  //   this->pattern = BAYER_PATTERN_GBRG;
  //   break;
  // case 0x59595959:      /* YYYY = BW */
  //   throw open_camera_error("Camera is black and white");
  // default:
  //   throw open_camera_error("Camera BAYER_TILE_MAPPING register has an unexpected value");
  // }

  this->grabFrameThread.reset(new boost::thread(&DC1394::doWork,this));
}

DC1394::~DC1394()
{
  if (this->grabFrameThread)
  {
    this->grabFrameThread->interrupt();
    this->grabFrameThread->join();
  }

  dc1394_stop_iso_transmission(this->raw1394Handle, this->dc1394Camera.node);

  if (this->bDc1394CameraCreated)
  {
    dc1394_dma_unlisten(this->raw1394Handle, &(this->dc1394Camera));
    dc1394_dma_release_camera(this->raw1394Handle, &(this->dc1394Camera));
  }

  if (this->raw1394Handle)
  {
    raw1394_destroy_handle(this->raw1394Handle);
  }
}

void
DC1394::findThisCamera(nodeid_t& node, int& cardIndex)
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
        cardIndex = iCard;
        return;
      }
    }

    raw1394_destroy_handle(this->raw1394Handle);
    this->raw1394Handle = NULL;
  }

  throw open_camera_error("Don't found a camera with uid " + this->uid);
}

std::string
DC1394::findVideo1394Device(unsigned cardNumber)
{
  std::string strCardNumber(boost::lexical_cast<std::string>(cardNumber));

  boost::array<std::string, 3> possibleDevices;
  possibleDevices.at(0) = "/dev/video1394/" + strCardNumber;
  possibleDevices.at(1) = "/dev/video1394-" + strCardNumber;
  possibleDevices.at(2) = "/dev/video1394" + strCardNumber;

  struct stat statbuf;
  std::string device;
  BOOST_FOREACH (device, possibleDevices)
  {
    if (stat(device.c_str(), &statbuf) == 0 && S_ISCHR(statbuf.st_mode))
    {
      return device;
    }
  }

  // If here, didn't found camera devide
  throw open_camera_error("Don't found the device of camera with uid " +
                          this->uid);
}

int
DC1394::getDc1394FrameRate(float frameRate) const
{
  if (frameRate == 1.875)
  {
    return FRAMERATE_1_875;
  }
  else if (frameRate == 3.75)
  {
    return FRAMERATE_3_75;
  }
  else if (frameRate == 7.5)
  {
    return FRAMERATE_7_5;
  }
  else if (frameRate == 15.0)
  {
    return FRAMERATE_15;
  }
  else if (frameRate == 30.0)
  {
    return FRAMERATE_30;
  }
  else if (frameRate == 60.0)
  {
    return FRAMERATE_60;
  }
  else if (frameRate == 120.0)
  {
    return FRAMERATE_120;
  }
  else if (frameRate == 240.0)
  {
    return FRAMERATE_240;
  }
  else
  {
    throw open_camera_error("Invalid frame rate " +
                            boost::lexical_cast<std::string>(frameRate));
  }
}

unsigned
DC1394::getDc1394VideoMode(const std::string& colorMode) const
{
  const unsigned fw = this->frameSize.width;
  const unsigned fh = this->frameSize.height;
  const std::string& cm = colorMode;

  if      (fw==640  && fh==480  && cm=="yuv444") return MODE_160x120_YUV444;
  else if (fw==320  && fh==240  && cm=="yuv422") return MODE_320x240_YUV422;
  else if (fw==640  && fh==480  && cm=="yuv411") return MODE_640x480_YUV411;
  else if (fw==640  && fh==480  && cm=="yuv422") return MODE_640x480_YUV422;
  else if (fw==640  && fh==480  && cm=="rgb8"  ) return MODE_640x480_RGB;
  else if (fw==640  && fh==480  && cm=="mono8" ) return MODE_640x480_MONO;
  else if (fw==640  && fh==480  && cm=="mono16") return MODE_640x480_MONO16;
  else if (fw==800  && fh==600  && cm=="yuv422") return MODE_800x600_YUV422;
  else if (fw==800  && fh==600  && cm=="rgb8"  ) return MODE_800x600_RGB;
  else if (fw==800  && fh==600  && cm=="mono8" ) return MODE_800x600_MONO;
  else if (fw==1024 && fh==768  && cm=="yuv422") return MODE_1024x768_YUV422;
  else if (fw==1024 && fh==768  && cm=="rgb8"  ) return MODE_1024x768_RGB;
  else if (fw==1024 && fh==768  && cm=="mono8" ) return MODE_1024x768_MONO;
  else if (fw==800  && fh==600  && cm=="mono16") return MODE_800x600_MONO16;
  else if (fw==1024 && fh==768  && cm=="mono16") return MODE_1024x768_MONO16;
  else if (fw==1280 && fh==960  && cm=="yuv422") return MODE_1280x960_YUV422;
  else if (fw==1280 && fh==960  && cm=="rgb8"  ) return MODE_1280x960_RGB;
  else if (fw==1280 && fh==960  && cm=="mono8" ) return MODE_1280x960_MONO;
  else if (fw==1600 && fh==1200 && cm=="yuv422") return MODE_1600x1200_YUV422;
  else if (fw==1600 && fh==1200 && cm=="rgb8"  ) return MODE_1600x1200_RGB;
  else if (fw==1600 && fh==1200 && cm=="mono8" ) return MODE_1600x1200_MONO;
  else if (fw==1280 && fh==960  && cm=="mono16") return MODE_1280x960_MONO16;
  else if (fw==1600 && fh==1200 && cm=="mono16") return MODE_1600x1200_MONO16;
  else
  {
    throw open_camera_error("Invalid resolution " +
                            boost::lexical_cast<std::string>(fw) +
                            " x " +
                            boost::lexical_cast<std::string>(fh)+
                            " with color mode " + colorMode);
  }
}

std::pair<int,int>
DC1394::getPairDepthChannels(const std::string& colorMode) const
{
  if (colorMode == "yuv444")
  {
    return std::make_pair(IPL_DEPTH_8U,3);
  }
  else if (colorMode == "yuv422")
  {
    return std::make_pair(IPL_DEPTH_8U,3);
  }
  else if (colorMode == "yuv411")
  {
    return std::make_pair(IPL_DEPTH_8U,3);
  }
  else if (colorMode == "rgb8")
  {
    return std::make_pair(IPL_DEPTH_8U,3);
  }
  else if (colorMode == "mono8") // There aren't supporto to bw camera, TODO?
  {
    return std::make_pair(IPL_DEPTH_8U,3);
  }
  else if (colorMode == "mono16") // There aren't supporto to bw camera, TODO?
  {
    return std::make_pair(IPL_DEPTH_16U,3);
  }
  else
  {
    throw std::invalid_argument("Invalid color mode: " + colorMode);
  }
}

int
DC1394::getIplDepth(const std::string& colorMode) const
{
  return getPairDepthChannels(colorMode).first;
}

int
DC1394::getNChannels(const std::string& colorMode) const
{
  return getPairDepthChannels(colorMode).second;
}

bool
DC1394::setOnOffFeature(nodeid_t cameraNode, dc1394_feature_info featureInfo,
                        std::string strValue, std::string featureName)
{
  if (featureInfo.on_off_capable)
  {
    dc1394bool_t onMode = (strValue=="off") ? DC1394_FALSE : DC1394_TRUE;
    if (dc1394_feature_on_off(this->raw1394Handle, cameraNode,
                              featureInfo.feature_id, onMode)
        == DC1394_FAILURE)
    {
      throw camera_parameter_error("Error on setting on/off mode to " +
                                   featureName + " feature of dc1394 camera "
                                   "uid: " + this->uid);
    }

    if (onMode == DC1394_TRUE)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if (strValue == "off") //Is not on/off capable and tried set off
  {
    throw camera_parameter_error("Trying set on/off mode to " + featureName +
                                 " feature of dc1394 camera uid: " + this->uid
                                 + ", but the camera is not capable.");
  }

  // If here the feature cannot be set off
  return true;
}

bool
DC1394::setAutoFeature(nodeid_t cameraNode, dc1394_feature_info featureInfo,
                       std::string strValue, std::string featureName)
{
  if (featureInfo.auto_capable)
  {
    dc1394bool_t autoMode = (strValue=="auto") ? DC1394_TRUE : DC1394_FALSE;
    if (dc1394_auto_on_off(this->raw1394Handle, cameraNode,
                           featureInfo.feature_id, autoMode)
        != DC1394_SUCCESS)
    {
      throw camera_parameter_error("Error on setting auto mode to " +
                                   featureName + " feature of dc1394 camera "
                                   "uid: " + this->uid);
    }

    if (autoMode == DC1394_TRUE)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if (strValue == "auto") // Is not auto capable and tried set auto mode
  {
    throw camera_parameter_error("Trying set auto mode to " + featureName +
                                 " feature of dc1394 camera uid: " + this->uid
                                 + ", but the camera is not capable.");
  }

  //If here the feature cannot be set auto
  return false;
}

void
DC1394::setFeatureValue(nodeid_t cameraNode, dc1394_feature_info featureInfo,
                        std::string strValue, std::string featureName)
{
  unsigned value;
  try
  {
    value = boost::lexical_cast<unsigned>(strValue);
  }
  catch (const boost::bad_lexical_cast& error)
  {
    throw camera_parameter_error("Invalid value of " + featureName + " feature"
                                 " of dc1394 camera uid: " + this->uid);
  }

  if(value<(unsigned)featureInfo.min || value>(unsigned)featureInfo.max)
  {
    throw camera_parameter_error
      ("The value of " + featureName + " feature of dc1394 camera uid " +
       this->uid + " must be between " +
       boost::lexical_cast<std::string>(featureInfo.min) + " and " +
       boost::lexical_cast<std::string>(featureInfo.max));
  }

  if (dc1394_set_feature_value(this->raw1394Handle, cameraNode,
                               featureInfo.feature_id, value)
      != DC1394_SUCCESS)
  {
    throw camera_parameter_error("Error on setting " + featureName + " feature"
                                 " to dc1394 camera uid: " + this->uid);
  }
}

void
DC1394::setWhiteBalance(nodeid_t cameraNode, dc1394_feature_info featureInfo,
                        std::string blueU, std::string redV)
{
  unsigned valueB, valueR;
  try
  {
    valueB = boost::lexical_cast<unsigned>(blueU);
    valueR = boost::lexical_cast<unsigned>(redV);
  }
  catch (const boost::bad_lexical_cast& error)
  {
    throw camera_parameter_error("Invalid value in white balance feature "
                                 "of dc1394 camera uid: " + this->uid);
  }

  if(valueB<(unsigned)featureInfo.min || valueB>(unsigned)featureInfo.max ||
     valueR<(unsigned)featureInfo.min || valueR>(unsigned)featureInfo.max)
  {
    throw camera_parameter_error
      ("The values in white balance feature of dc1394 camera uid " +
       this->uid + " must be between " +
       boost::lexical_cast<std::string>(featureInfo.min) + " and " +
       boost::lexical_cast<std::string>(featureInfo.max));
  }

  if (dc1394_set_white_balance(this->raw1394Handle, cameraNode, valueB, valueR)
      != DC1394_SUCCESS)
  {
    throw open_camera_error("Error on setting white balance to dc1394 "
                            "camera uid: " + this->uid);
  }
}

void
DC1394::setWhiteShading(nodeid_t cameraNode, dc1394_feature_info featureInfo,
                        std::string red, std::string green, std::string blue)
{
  unsigned valueR, valueG, valueB;
  try
  {
    valueR = boost::lexical_cast<unsigned>(red);
    valueG = boost::lexical_cast<unsigned>(green);
    valueB = boost::lexical_cast<unsigned>(blue);
  }
  catch (const boost::bad_lexical_cast& error)
  {
    throw camera_parameter_error("Invalid value in white shading feature "
                                 "of dc1394 camera uid: " + this->uid);
  }

  if(valueR<(unsigned)featureInfo.min || valueR>(unsigned)featureInfo.max ||
     valueG<(unsigned)featureInfo.min || valueG>(unsigned)featureInfo.max ||
     valueB<(unsigned)featureInfo.min || valueB>(unsigned)featureInfo.max)
  {
    throw camera_parameter_error
      ("The values in white shading feature of dc1394 camera uid " +
       this->uid + " must be between " +
       boost::lexical_cast<std::string>(featureInfo.min) + " and " +
       boost::lexical_cast<std::string>(featureInfo.max));
  }

  if (dc1394_set_white_shading(this->raw1394Handle, cameraNode,
                               valueR, valueG, valueB)
      != DC1394_SUCCESS)
  {
    throw open_camera_error("Error on setting white shading to dc1394 "
                            "camera uid: " + this->uid);
  }
}

void
DC1394::setFeatures(const scene::DC1394& config, nodeid_t cameraNode)
{
  typedef boost::tuple<unsigned,
                       boost::optional<std::string>,
                       std::string> FeatureTuple;

  std::vector<FeatureTuple > features;
  features += FeatureTuple(FEATURE_BRIGHTNESS, config.brightness,
                           "brightness"),
              FeatureTuple(FEATURE_EXPOSURE, config.exposure, "exposure"),
              FeatureTuple(FEATURE_SHARPNESS, config.sharpness, "sharpness"),
              FeatureTuple(FEATURE_HUE, config.hue, "hue"),
              FeatureTuple(FEATURE_SATURATION, config.saturation,
                           "saturation"),
              FeatureTuple(FEATURE_GAMMA, config.gamma, "gamma"),
              FeatureTuple(FEATURE_SHUTTER, config.shutter, "shutter"),
              FeatureTuple(FEATURE_GAIN, config.gain, "gain"),
              FeatureTuple(FEATURE_IRIS, config.iris, "iris"),
              FeatureTuple(FEATURE_FOCUS, config.focus, "focus"),
              FeatureTuple(FEATURE_TEMPERATURE, config.temperature,
                           "temperature"),
              FeatureTuple(FEATURE_TRIGGER, config.trigger, "trigger"),
              FeatureTuple(FEATURE_TRIGGER_DELAY, config.triggerDelay,
                           "trigger delay"),
              FeatureTuple(FEATURE_ZOOM, config.zoom, "zoom"),
              FeatureTuple(FEATURE_PAN, config.pan, "pan"),
              FeatureTuple(FEATURE_TILT, config.tilt, "tilt"),
              FeatureTuple(FEATURE_OPTICAL_FILTER, config.opticalFilter,
                           "optical filter"),
              FeatureTuple(FEATURE_CAPTURE_SIZE, config.captureSize,
                           "capture size"),
              FeatureTuple(FEATURE_CAPTURE_QUALITY, config.captureQuality,
                           "capture quality");

  FeatureTuple featureTuple;
  BOOST_FOREACH(featureTuple, features)
  {
    if (!featureTuple.get<1>()) // If feature was not given
    {
      continue;
    }

    std::string strValue(*featureTuple.get<1>());
    std::string featureName(featureTuple.get<2>());

    dc1394_feature_info featureInfo;
    featureInfo.feature_id = featureTuple.get<0>();
    if (dc1394_get_camera_feature(this->raw1394Handle,
                                  cameraNode, &featureInfo)
        == DC1394_FAILURE)
    {
      throw camera_parameter_error("Could not get information about " +
                                   featureName + " feature of dc1394 camera "
                                   "uid: " + this->uid);
    }

    if (!featureInfo.available)
    {
      throw camera_parameter_error("The " + featureName + " feature is not "
                                   "avaible in dc1394 camera uid: " +
                                   this->uid);
    }

    bool isOn(false);
    bool isAuto(false);
    isOn = setOnOffFeature(cameraNode, featureInfo, strValue, featureName);
    if (isOn)
    {
      isAuto = setAutoFeature(cameraNode, featureInfo, strValue, featureName);
    }
    if (isOn && !isAuto)
    {
      setFeatureValue(cameraNode, featureInfo, strValue, featureName);
    }
  }

  if (config.whiteBalance)
  {
    dc1394_feature_info featureInfo;
    featureInfo.feature_id = FEATURE_WHITE_BALANCE;
    if (dc1394_get_camera_feature(this->raw1394Handle,
                                  cameraNode, &featureInfo)
        == DC1394_FAILURE)
    {
      throw camera_parameter_error("Could not get information about white "
                                   "balance feature of dc1394 camera uid: " +
                                   this->uid);
    }

    if (!featureInfo.available)
    {
      throw camera_parameter_error("The white balance feature is not "
                                   "avaible in dc1394 camera uid: " +
                                   this->uid);
    }

    bool isOn(false);
    bool isAuto(false);
    isOn = setOnOffFeature(cameraNode, featureInfo, config.whiteBalance->at(0),
                           "white balance");
    if (isOn)
    {
      isAuto = setAutoFeature(cameraNode, featureInfo,
                              config.whiteBalance->at(0), "white balance");
    }
    if (isOn && !isAuto)
    {
      setWhiteBalance(cameraNode, featureInfo, config.whiteBalance->at(0),
                      config.whiteBalance->at(1));
    }
  }

  if (config.whiteShading)
  {
    dc1394_feature_info featureInfo;
    featureInfo.feature_id = FEATURE_WHITE_SHADING;
    if (dc1394_get_camera_feature(this->raw1394Handle,
                                  cameraNode, &featureInfo)
        == DC1394_FAILURE)
    {
      throw camera_parameter_error("Could not get information about white "
                                   "shading feature of dc1394 camera uid: " +
                                   this->uid);
    }

    if (!featureInfo.available)
    {
      throw camera_parameter_error("The white shading feature is not "
                                   "avaible in dc1394 camera uid: " +
                                   this->uid);
    }

    bool isOn(false);
    bool isAuto(false);
    isOn = setOnOffFeature(cameraNode, featureInfo, config.whiteShading->at(0),
                           "white shading");
    if (isOn)
    {
      isAuto = setAutoFeature(cameraNode, featureInfo,
                              config.whiteShading->at(0), "white shading");
    }
    if (isOn && !isAuto)
    {
      setWhiteShading(cameraNode, featureInfo, config.whiteShading->at(0),
                      config.whiteShading->at(1), config.whiteShading->at(2));
    }
  }
}

void
DC1394::doWork()
{
  IplImageWrapper tmpFrame(this->frameSize, this->iplDepth, 3);
  boost::shared_ptr<IplImageWrapper> frameBuffer[2];
  int frameIdx = 0;

  frameBuffer[0].reset(new IplImageWrapper(this->frameSize,
                                           this->iplDepth, 3));
  frameBuffer[1].reset(new IplImageWrapper(this->frameSize,
                                           this->iplDepth, 3));

  while (true)
  {
    boost::this_thread::interruption_point();

    if (dc1394_dma_single_capture(&(this->dc1394Camera)) != DC1394_SUCCESS)
    {
      // throw capture_image_error("Unable to capture a frame");
      dc1394_dma_done_with_buffer(&(this->dc1394Camera));
      continue;
    }

    dc1394_bayer_decoding_8bit((uint8_t*)this->dc1394Camera.capture_buffer,
                               (uint8_t*)tmpFrame.data(),
                               this->dc1394Camera.frame_width,
                               this->dc1394Camera.frame_height,
                               LIBDC1394_COLOR_FILTER_RGGB,
                               LIBDC1394_BAYER_METHOD_SIMPLE);

    dc1394_dma_done_with_buffer(&(this->dc1394Camera));

    tmpFrame.convertColor(*frameBuffer[frameIdx], CV_RGB2BGR);
    updateCurrentFrame(frameBuffer[frameIdx]);

    frameIdx = (frameIdx+1) % 2;
  }
}
