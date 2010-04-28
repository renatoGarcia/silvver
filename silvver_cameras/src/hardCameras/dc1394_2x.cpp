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

#include <boost/array.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <map>
#include <stdint.h>
#include <utility>

#include "../exceptions.hpp"
#include "../frame.hpp"
#include "../log.hpp"

using namespace boost::assign;

DC1394::DC1394(const scene::DC1394& config)
  :HardCamera(config, DC1394::getIplDepth(config.colorMode))
  ,uid(config.uid)
  ,videoMode(DC1394::getDc1394VideoMode(config))
  ,context(NULL)
  ,camera(NULL)
  ,grabFrameThread()
  ,colorConverter(DC1394::createColorConverter(config))
{
  this->context = dc1394_new();
  if (!this->context)
  {
    throw open_camera_error()
      << info_what("Unable to create a libdc1394 context")
      << info_cameraUid(this->silvverUid);
  }

  this->camera = dc1394_camera_new(this->context,
                                   boost::lexical_cast<uint64_t>(this->uid));
  if (!this->camera)
  {
    throw open_camera_error()
      << info_what("Don't found specified camera")
      << info_cameraUid(this->silvverUid);
  }

  if (dc1394_video_set_iso_speed(this->camera, DC1394_ISO_SPEED_400))
  {
    throw open_camera_error()
      << info_what("Could not set 400Mbps iso speed")
      << info_cameraUid(this->silvverUid);
  }

  if (dc1394_video_set_mode(camera, this->videoMode))
  {
    throw open_camera_error()
      << info_what("Could not set video mode")
      << info_cameraUid(this->silvverUid);
  }

  if (dc1394_video_set_framerate(camera,
                                 DC1394::getDc1394FrameRate(config.frameRate)))
  {
    throw open_camera_error()
      << info_what("Could not set framerate")
      << info_cameraUid(this->silvverUid);
  }

  if (dc1394_capture_setup(this->camera, DC1394::N_BUFFERS,
                           DC1394_CAPTURE_FLAGS_DEFAULT))
  {
    throw open_camera_error()
      << info_what("Could not setup camera. Make sure that the resolution and framerate are supported by your camera")
      << info_cameraUid(this->silvverUid);
  }

  try
  {
    this->setFeatures(config);
  }
  catch(silvver_cameras_exception& e)
  {
    throw e << info_cameraUid(this->silvverUid);
  }

  if (dc1394_video_set_transmission(this->camera, DC1394_ON))
  {
    throw open_camera_error()
      << info_what("Could not start camera iso transmission")
      << info_cameraUid(this->silvverUid);
  }
  this->grabFrameThread.reset(new boost::thread(&DC1394::doWork, this));
}

DC1394::~DC1394()
{
  if (this->grabFrameThread)
  {
    this->grabFrameThread->interrupt();
    this->grabFrameThread->join();
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
DC1394::getDc1394VideoMode(const scene::DC1394& config)
{
  const unsigned fw = config.resolution.at(0);
  const unsigned fh = config.resolution.at(1);
  const std::string& cm = config.colorMode;

  if      (fw==640  && fh==480  && cm=="yuv444")
    return DC1394_VIDEO_MODE_160x120_YUV444;
  else if (fw==320  && fh==240  && cm=="yuv422")
    return DC1394_VIDEO_MODE_320x240_YUV422;
  else if (fw==640  && fh==480  && cm=="yuv411")
    return DC1394_VIDEO_MODE_640x480_YUV411;
  else if (fw==640  && fh==480  && cm=="yuv422")
    return DC1394_VIDEO_MODE_640x480_YUV422;
  else if (fw==640  && fh==480  && cm=="rgb8"  )
    return DC1394_VIDEO_MODE_640x480_RGB8;
  else if (fw==640  && fh==480  && cm=="mono8" )
    return DC1394_VIDEO_MODE_640x480_MONO8;
  else if (fw==640  && fh==480  && cm=="mono16")
    return DC1394_VIDEO_MODE_640x480_MONO16;
  else if (fw==800  && fh==600  && cm=="yuv422")
    return DC1394_VIDEO_MODE_800x600_YUV422;
  else if (fw==800  && fh==600  && cm=="rgb8"  )
    return DC1394_VIDEO_MODE_800x600_RGB8;
  else if (fw==800  && fh==600  && cm=="mono8" )
    return DC1394_VIDEO_MODE_800x600_MONO8;
  else if (fw==1024 && fh==768  && cm=="yuv422")
    return DC1394_VIDEO_MODE_1024x768_YUV422;
  else if (fw==1024 && fh==768  && cm=="rgb8"  )
    return DC1394_VIDEO_MODE_1024x768_RGB8;
  else if (fw==1024 && fh==768  && cm=="mono8" )
    return DC1394_VIDEO_MODE_1024x768_MONO8;
  else if (fw==800  && fh==600  && cm=="mono16")
    return DC1394_VIDEO_MODE_800x600_MONO16;
  else if (fw==1024 && fh==768  && cm=="mono16")
    return DC1394_VIDEO_MODE_1024x768_MONO16;
  else if (fw==1280 && fh==960  && cm=="yuv422")
    return DC1394_VIDEO_MODE_1280x960_YUV422;
  else if (fw==1280 && fh==960  && cm=="rgb8"  )
    return DC1394_VIDEO_MODE_1280x960_RGB8;
  else if (fw==1280 && fh==960  && cm=="mono8" )
    return DC1394_VIDEO_MODE_1280x960_MONO8;
  else if (fw==1600 && fh==1200 && cm=="yuv422")
    return DC1394_VIDEO_MODE_1600x1200_YUV422;
  else if (fw==1600 && fh==1200 && cm=="rgb8"  )
    return DC1394_VIDEO_MODE_1600x1200_RGB8;
  else if (fw==1600 && fh==1200 && cm=="mono8" )
    return DC1394_VIDEO_MODE_1600x1200_MONO8;
  else if (fw==1280 && fh==960  && cm=="mono16")
    return DC1394_VIDEO_MODE_1280x960_MONO16;
  else if (fw==1600 && fh==1200 && cm=="mono16")
    return DC1394_VIDEO_MODE_1600x1200_MONO16;
  else
  {
    throw open_camera_error()
      << info_what("Could not set resolution and color mode together")
      << info_cameraUid(config.uid)
      << info_resolution(config.resolution)
      << info_colorMode(config.colorMode);
  }
}

int
DC1394::getIplDepth(const std::string& colorMode)
{
  if (colorMode == "yuv444")
  {
    return IPL_DEPTH_8U;
  }
  else if (colorMode == "yuv422")
  {
    return IPL_DEPTH_8U;
  }
  else if (colorMode == "yuv411")
  {
    return IPL_DEPTH_8U;
  }
  else if (colorMode == "rgb8")
  {
    return IPL_DEPTH_8U;
  }
  else if (colorMode == "mono8")
  {
    return IPL_DEPTH_8U;
  }
  else if (colorMode == "mono16")
  {
    return IPL_DEPTH_16U;
  }
  else
  {
    throw invalid_argument()
      << info_what("Invalid color mode")
      << info_colorMode(colorMode);
  }
}

dc1394framerate_t
DC1394::getDc1394FrameRate(const float frameRate)
{
  if (frameRate == 1.875)
  {
    return DC1394_FRAMERATE_1_875;
  }
  else if (frameRate == 3.75)
  {
    return DC1394_FRAMERATE_3_75;
  }
  else if (frameRate == 7.5)
  {
    return DC1394_FRAMERATE_7_5;
  }
  else if (frameRate == 15.0)
  {
    return DC1394_FRAMERATE_15;
  }
  else if (frameRate == 30.0)
  {
    return DC1394_FRAMERATE_30;
  }
  else if (frameRate == 60.0)
  {
    return DC1394_FRAMERATE_60;
  }
  else if (frameRate == 120.0)
  {
    return DC1394_FRAMERATE_120;
  }
  else if (frameRate == 240.0)
  {
    return DC1394_FRAMERATE_240;
  }
  else
  {
    throw open_camera_error()
      << info_what("Invalid frame rate")
      << info_frameRate(frameRate);
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
      throw e << info_cameraUid(config.uid)
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
      throw e << info_cameraUid(config.uid)
              << info_colorMode(config.colorMode);
    }
  }
}

bool
DC1394::setPower(dc1394feature_info_t featureInfo,
                 std::string strValue, std::string featureName)
{
  if (featureInfo.on_off_capable)
  {
    dc1394switch_t power = (strValue=="off") ? DC1394_OFF : DC1394_ON;
    if (dc1394_feature_set_power(this->camera, featureInfo.id, power))
    {
      throw camera_parameter_error()
        << info_what("Error on setting on/off mode of a feature")
        << info_featureName(featureName);
    }

    if (power == DC1394_ON)
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
DC1394::setAutoMode(dc1394feature_info_t featureInfo,
                    std::string strValue, std::string featureName)
{
  dc1394feature_modes_t featureModes;
  if (dc1394_feature_get_modes(this->camera, featureInfo.id, &featureModes))
  {
    throw camera_parameter_error()
      << info_what("Error on queryng auto mode capability of a feature")
      << info_featureName(featureName);
  }

  dc1394bool_t hasAutoMode = DC1394_FALSE;
  for(unsigned i = 0; i < featureModes.num; ++i)
  {
    if (featureModes.modes[i] == DC1394_FEATURE_MODE_AUTO)
    {
      hasAutoMode = DC1394_TRUE;
    }
  }

  if (hasAutoMode)
  {
    dc1394feature_mode_t mode = (strValue=="auto") ?
                        DC1394_FEATURE_MODE_AUTO : DC1394_FEATURE_MODE_MANUAL;
    if (dc1394_feature_set_mode(this->camera, featureInfo.id, mode))
    {
      throw camera_parameter_error()
        << info_what("Error on setting auto mode to a feature")
        << info_featureName(featureName);
    }

    if (mode == DC1394_FEATURE_MODE_AUTO)
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
DC1394::setFeatureValue(dc1394feature_info_t featureInfo,
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

  if (dc1394_feature_set_value(this->camera, featureInfo.id, value))
  {
    throw camera_parameter_error()
      << info_what("Error on setting feature")
      << info_featureName(featureName);
  }
}

void
DC1394::setWhiteBalance(dc1394feature_info_t featureInfo,
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

  if (dc1394_feature_whitebalance_set_value(this->camera, valueB, valueR))
  {
    throw camera_parameter_error()
      << info_what("Error on setting feature")
      << info_featureName("White balance");
  }
}

void
DC1394::setWhiteShading(dc1394feature_info_t featureInfo,
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

  if (dc1394_feature_whiteshading_set_value(this->camera,
                                            valueR, valueG, valueB))
  {
    throw camera_parameter_error()
      << info_what("Error on setting feature")
      << info_featureName("White shading");
  }
}

void
DC1394::setFeatures(const scene::DC1394& config)
{
  typedef boost::tuple<dc1394feature_t,
                       boost::optional<std::string>,
                       std::string> FeatureTuple;

  std::vector<FeatureTuple > features;
  features += FeatureTuple(DC1394_FEATURE_BRIGHTNESS,
                           config.brightness,
                           "brightness"),
              FeatureTuple(DC1394_FEATURE_EXPOSURE,
                           config.exposure,
                           "exposure"),
              FeatureTuple(DC1394_FEATURE_SHARPNESS,
                           config.sharpness,
                           "sharpness"),
              FeatureTuple(DC1394_FEATURE_HUE,
                           config.hue,
                           "hue"),
              FeatureTuple(DC1394_FEATURE_SATURATION,
                           config.saturation,
                           "saturation"),
              FeatureTuple(DC1394_FEATURE_GAMMA,
                           config.gamma,
                           "gamma"),
              FeatureTuple(DC1394_FEATURE_SHUTTER,
                           config.shutter,
                           "shutter"),
              FeatureTuple(DC1394_FEATURE_GAIN,
                           config.gain,
                           "gain"),
              FeatureTuple(DC1394_FEATURE_IRIS,
                           config.iris,
                           "iris"),
              FeatureTuple(DC1394_FEATURE_FOCUS,
                           config.focus,
                           "focus"),
              FeatureTuple(DC1394_FEATURE_TEMPERATURE,
                           config.temperature,
                           "temperature"),
              FeatureTuple(DC1394_FEATURE_TRIGGER,
                           config.trigger,
                           "trigger"),
              FeatureTuple(DC1394_FEATURE_TRIGGER_DELAY,
                           config.triggerDelay,
                           "trigger delay"),
              FeatureTuple(DC1394_FEATURE_ZOOM,
                           config.zoom,
                           "zoom"),
              FeatureTuple(DC1394_FEATURE_PAN,
                           config.pan,
                           "pan"),
              FeatureTuple(DC1394_FEATURE_TILT,
                           config.tilt,
                           "tilt"),
              FeatureTuple(DC1394_FEATURE_OPTICAL_FILTER,
                           config.opticalFilter,
                           "optical filter"),
              FeatureTuple(DC1394_FEATURE_CAPTURE_SIZE,
                           config.captureSize,
                           "capture size"),
              FeatureTuple(DC1394_FEATURE_CAPTURE_QUALITY,
                           config.captureQuality,
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

    dc1394feature_info_t featureInfo;
    featureInfo.id = featureTuple.get<0>();
    if (dc1394_feature_get(this->camera, &featureInfo))
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
    isOn = setPower(featureInfo, strValue, featureName);
    if (isOn)
    {
      isAuto = setAutoMode(featureInfo, strValue, featureName);
    }
    if (isOn && !isAuto)
    {
      setFeatureValue(featureInfo, strValue, featureName);
    }
  }

  if (config.whiteBalance)
  {
    dc1394feature_info_t featureInfo;
    featureInfo.id = DC1394_FEATURE_WHITE_BALANCE;
    if (dc1394_feature_get(this->camera, &featureInfo))
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
    isOn = setPower(featureInfo, config.whiteBalance->at(0), "white balance");
    if (isOn)
    {
      isAuto = setAutoMode(featureInfo, config.whiteBalance->at(0),
                           "white balance");
    }
    if (isOn && !isAuto)
    {
      setWhiteBalance(featureInfo, config.whiteBalance->at(0),
                      config.whiteBalance->at(1));
    }
  }

  if (config.whiteShading)
  {
    dc1394feature_info_t featureInfo;
    featureInfo.id = DC1394_FEATURE_WHITE_SHADING;
    if (dc1394_feature_get(this->camera, &featureInfo))
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
    isOn = setPower(featureInfo, config.whiteShading->at(0), "white shading");
    if (isOn)
    {
      isAuto = setAutoMode(featureInfo, config.whiteShading->at(0),
                           "white shading");
    }
    if (isOn && !isAuto)
    {
      setWhiteShading(featureInfo, config.whiteShading->at(0),
                      config.whiteShading->at(1), config.whiteShading->at(2));
    }
  }
}

void
DC1394::doWork()
{
  dc1394video_frame_t* videoFrame = NULL;
  boost::array<Frame, 2> frameBuffer;
  int index = 0;

  frameBuffer[0].image = IplImageWrapper(this->frameSize, this->iplDepth, 3);
  frameBuffer[1].image = IplImageWrapper(this->frameSize, this->iplDepth, 3);

  while (true)
  {
    boost::this_thread::interruption_point();

    if (dc1394_capture_dequeue(this->camera,
                               DC1394_CAPTURE_POLICY_WAIT,
                               &videoFrame))
    {
      message(LogLevel::ERROR)
        << ts_output::lock
        << "Could not capture a frame in dc1394 camera uid: "
        << this->uid << std::endl
        << ts_output::unlock;
    }

    frameBuffer[index].timestamp = videoFrame->timestamp;
    this->colorConverter((uint8_t*)videoFrame->image,
                         frameBuffer[index].image);

    updateCurrentFrame(frameBuffer[index]);

    // Release the buffer
    if (dc1394_capture_enqueue(this->camera, videoFrame))
    {
      message(LogLevel::ERROR)
        << ts_output::lock
        << "Error on returning a frame to ring buffer in dc1394 camera uid: "
        << this->uid << std::endl
        << ts_output::unlock;
    }

    index = (index+1) % 2;
  }
}
