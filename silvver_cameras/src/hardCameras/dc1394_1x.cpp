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

#include <boost/array.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <cstddef>
#include <stdint.h>

#include <sys/stat.h>

#include "../exceptions.hpp"
#include "../iplImageWrapper.hpp"
#include "../log.hpp"

using namespace boost::assign;

DC1394::DC1394(const scene::DC1394& config)
  :HardCamera(config, DC1394::getIplDepth(config.colorMode))
  ,uid(config.uid)
  ,raw1394Handle(NULL)
  ,bDc1394CameraCreated(false)
  ,videoMode(DC1394::getDc1394VideoMode(config))
  ,colorConverter(DC1394::createColorConverter(config))
  ,grabFrameThread()
{
  nodeid_t cameraNode;
  int cardIndex;
  findThisCamera(cameraNode, cardIndex);

  int dc1394FrameRate = DC1394::getDc1394FrameRate(config.frameRate);

  unsigned int channel;
  unsigned int speed;
  if (dc1394_get_iso_channel_and_speed(this->raw1394Handle,
                                       cameraNode,
                                       &channel,
                                       &speed ) !=DC1394_SUCCESS)
  {
    throw open_camera_error()
      << info_what("Unable to get the iso channel number")
      << info_cameraUid(this->silvverUid);
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
    throw open_camera_error()
      << info_what("Unable to setup camera")
      << info_cameraUid(this->silvverUid);
  }
  else
  {
    this->bDc1394CameraCreated = true;
  }

  try
  {
    this->setFeatures(config, cameraNode);
  }
  catch(silvver_cameras_exception& e)
  {
    throw e << info_cameraUid(this->silvverUid);
  }

  // Have the camera start sending us data
  if (dc1394_start_iso_transmission(this->raw1394Handle,
                                    this->dc1394Camera.node)
      != DC1394_SUCCESS)
  {
    throw open_camera_error()
      << info_what("Unable to start camera iso transmission")
      << info_cameraUid(this->silvverUid);
  }

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
      throw open_camera_error()
        << info_what("Unable to aquire a raw1394 handle")
        << info_cameraUid(this->silvverUid);
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

  throw open_camera_error()
    << info_what("Didn't found the camera with required uid")
    << info_cameraUid(this->silvverUid);
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
  throw open_camera_error()
    << info_what("Didn't found the path to camera device")
    << info_cameraUid(this->silvverUid);
}

int
DC1394::getDc1394FrameRate(float frameRate)
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
    throw open_camera_error()
      << info_what("Invalid frame rate")
      << info_frameRate(frameRate);
  }
}

unsigned
DC1394::getDc1394VideoMode(const scene::DC1394& config)
{
  const unsigned fw = config.resolution.at(0);
  const unsigned fh = config.resolution.at(1);
  const std::string& cm = config.colorMode;

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
    throw open_camera_error()
      << info_what("Could not set resolution and color mode together")
      << info_cameraUid(config.silvverUid)
      << info_resolution(config.resolution)
      << info_colorMode(config.colorMode);
  }
}

int
DC1394::getIplDepth(const std::string& colorMode)
{
  if (colorMode == "yuv444")      return IPL_DEPTH_8U;
  else if (colorMode == "yuv422") return IPL_DEPTH_8U;
  else if (colorMode == "yuv411") return IPL_DEPTH_8U;
  else if (colorMode == "rgb8")   return IPL_DEPTH_8U;
  else if (colorMode == "mono8")  return IPL_DEPTH_8U;
  else if (colorMode == "mono16") return IPL_DEPTH_16U;
  else
  {
    throw invalid_argument()
      << info_what("Invalid color mode")
      << info_colorMode(colorMode);
  }
}

ColorConverter
DC1394::createColorConverter(const scene::DC1394& config)
{
  std::map<std::string, ColorSpace> colorSpaceMap = map_list_of
    ("yuv411", ColorSpace::YUV411)
    ("yuyv"  , ColorSpace::YUYV)
    ("uyvy"  , ColorSpace::UYVY)
    ("rgb8"  , ColorSpace::RGB8)
    ("mono8" , ColorSpace::MONO8)
    ("mono16", ColorSpace::MONO16);

  std::map<std::string, BayerMethod> bayerMap = map_list_of
    ("nearest" , BayerMethod::NEAREST)
    ("bilinear", BayerMethod::BILINEAR);

  std::map<std::string, ColorFilter> colorFilterMap = map_list_of
    ("rggb", ColorFilter::RGGB)
    ("gbrg", ColorFilter::GBRG)
    ("grbg", ColorFilter::GRBG)
    ("bggr", ColorFilter::BGGR);

  if (config.bayerMethod)
  {
    try
    {
      if (config.colorMode == "mono8")
      {
        return ColorConverter(ColorSpace::RAW8,
                              config.resolution.at(0), config.resolution.at(1),
                              colorFilterMap[*(config.colorFilter)],
                              bayerMap[*(config.bayerMethod)]);
      }
      else if (config.colorMode == "mono16")
      {
        return ColorConverter(ColorSpace::RAW16,
                              config.resolution.at(0), config.resolution.at(1),
                              colorFilterMap[*(config.colorFilter)],
                              bayerMap[*(config.bayerMethod)]);
      }
    }
    catch (invalid_argument& e)
    {
      throw e << info_cameraUid(config.silvverUid)
              << info_colorMode(config.colorMode)
              << info_bayer(*(config.bayerMethod))
              << info_colorFilter(*(config.colorFilter));
    }
  }
  else
  {
    try
    {
      return ColorConverter(colorSpaceMap[config.colorMode],
                            config.resolution.at(0), config.resolution.at(1));
    }
    catch (invalid_argument& e)
    {
      throw e << info_cameraUid(config.silvverUid)
              << info_colorMode(config.colorMode);
    }
  }
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
      throw camera_parameter_error()
        << info_what("Error on setting on/off mode of a feature")
        << info_featureName(featureName);
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
    throw camera_parameter_error()
      << info_what("Feature is not on/off mode capable")
      << info_featureName(featureName);
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
      throw camera_parameter_error()
        << info_what("Error on setting auto mode to a feature")
        << info_featureName(featureName);
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
    throw camera_parameter_error()
      << info_what("Feature is not auto mode capable")
      << info_featureName(featureName);
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
    throw camera_parameter_error()
      << info_what("Invalid feature value type")
      << info_featureName(featureName);
  }

  if(value<(unsigned)featureInfo.min || value>(unsigned)featureInfo.max)
  {
    throw camera_parameter_error()
      << info_what("Feature value out of rage")
      << info_featureName(featureName)
      << info_featureRange(boost::lexical_cast<std::string>(featureInfo.min) +
                           '-' +
                           boost::lexical_cast<std::string>(featureInfo.max));
  }

  if (dc1394_set_feature_value(this->raw1394Handle, cameraNode,
                               featureInfo.feature_id, value)
      != DC1394_SUCCESS)
  {
    throw camera_parameter_error()
      << info_what("Error on setting feature")
      << info_featureName(featureName);
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
     throw camera_parameter_error()
      << info_what("Invalid feature value type")
      << info_featureName("White balance");
  }

  if(valueB<(unsigned)featureInfo.min || valueB>(unsigned)featureInfo.max ||
     valueR<(unsigned)featureInfo.min || valueR>(unsigned)featureInfo.max)
  {
    throw camera_parameter_error()
      << info_what("Feature value out of rage")
      << info_featureName("White balance")
      << info_featureRange(boost::lexical_cast<std::string>(featureInfo.min) +
                           '-' +
                           boost::lexical_cast<std::string>(featureInfo.max));
  }

  if (dc1394_set_white_balance(this->raw1394Handle, cameraNode, valueB, valueR)
      != DC1394_SUCCESS)
  {
    throw camera_parameter_error()
      << info_what("Error on setting feature")
      << info_featureName("White balance");
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
    throw camera_parameter_error()
      << info_what("Invalid feature value type")
      << info_featureName("White shading");
  }

  if(valueR<(unsigned)featureInfo.min || valueR>(unsigned)featureInfo.max ||
     valueG<(unsigned)featureInfo.min || valueG>(unsigned)featureInfo.max ||
     valueB<(unsigned)featureInfo.min || valueB>(unsigned)featureInfo.max)
  {
    throw camera_parameter_error()
      << info_what("Feature value out of rage")
      << info_featureName("White shading")
      << info_featureRange(boost::lexical_cast<std::string>(featureInfo.min) +
                           '-' +
                           boost::lexical_cast<std::string>(featureInfo.max));
  }

  if (dc1394_set_white_shading(this->raw1394Handle, cameraNode,
                               valueR, valueG, valueB)
      != DC1394_SUCCESS)
  {
    throw camera_parameter_error()
      << info_what("Error on setting feature")
      << info_featureName("White shading");
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
      throw camera_parameter_error()
        << info_what("Could not get feature informations")
        << info_featureName(featureName);
    }

    if (!featureInfo.available)
    {
      throw camera_parameter_error()
        << info_what("feature not available")
        << info_featureName(featureName);
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
      throw camera_parameter_error()
        << info_what("Could not get feature informations")
        << info_featureName("White balance");
    }

    if (!featureInfo.available)
    {
      throw camera_parameter_error()
        << info_what("Feature not available")
        << info_featureName("White balance");
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
      throw camera_parameter_error()
        << info_what("Could not get feature informations")
        << info_featureName("White shading");
    }

    if (!featureInfo.available)
    {
      throw camera_parameter_error()
        << info_what("Feature not available")
        << info_featureName("White shading");
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
  boost::array<Frame, 2> frameBuffer;
  int index = 0;

  frameBuffer[0].image = IplImageWrapper(this->frameSize, this->iplDepth, 3);
  frameBuffer[1].image = IplImageWrapper(this->frameSize, this->iplDepth, 3);

  while (true)
  {
    boost::this_thread::interruption_point();

    if (dc1394_dma_single_capture(&(this->dc1394Camera)) != DC1394_SUCCESS)
    {
      message(LogLevel::ERROR)
        << ts_output::lock
        << "Could not capture a frame in dc1394 camera uid: "
        << this->uid << std::endl
        << ts_output::unlock;

      dc1394_dma_done_with_buffer(&(this->dc1394Camera));
      continue;
    }

    frameBuffer[index].timestamp = this->dc1394Camera.filltime.tv_sec*1000000+
                                   this->dc1394Camera.filltime.tv_usec;
    this->colorConverter((uint8_t*)this->dc1394Camera.capture_buffer,
                         frameBuffer[index].image);

    dc1394_dma_done_with_buffer(&(this->dc1394Camera));

    updateCurrentFrame(frameBuffer[index]);

    index = (index+1) % 2;
  }
}
