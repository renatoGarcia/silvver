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
#include <boost/assign/std/vector.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include <stdint.h>

#include "../iplImageWrapper.hpp"

using namespace boost::assign;

DC1394::DC1394(const scene::DC1394& config)
  :HardCamera(config, getBitsPerPixel(config.colorMode))
  ,uid(config.uid)
  ,frameRate(config.frameRate)
  ,depth(getDepth(this->bitsPerPixel))
  ,videoMode(getDc1394VideoMode(config.colorMode))
  ,bufferSize((this->framePixels * this->bitsPerPixel) / 8)
  ,context(NULL)
  ,camera(NULL)
  ,grabFrameThread()
  ,brightness(config.brightness)
  ,exposure(config.exposure)
  ,whiteBalance(config.whiteBalance)
  ,shutter(config.shutter)
  ,gain(config.gain)
{
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

  if (this->camera)
  {
    dc1394_video_set_transmission(this->camera, DC1394_OFF);
    dc1394_capture_stop(this->camera);
    dc1394_camera_free(this->camera);
  }

  dc1394_free(this->context);
}

dc1394video_mode_t
DC1394::getDc1394VideoMode(const std::string& colorMode) const
{
  const unsigned fw = this->frameSize.width;
  const unsigned fh = this->frameSize.height;
  const std::string& cm = colorMode;

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
    throw open_camera_error("Invalid resolution " +
                            boost::lexical_cast<std::string>(fw) + " x " +
                            boost::lexical_cast<std::string>(fh) +
                            " with color mode " + colorMode);
  }
}

unsigned
DC1394::getBitsPerPixel(const std::string& colorMode) const
{
  if (colorMode == "yuv444")
  {
    return 12;
  }
  else if (colorMode == "yuv422")
  {
    return 8;
  }
  else if (colorMode == "yuv411")
  {
    return 6;
  }
  else if (colorMode == "rgb8")
  {
    return 8;
  }
  else if (colorMode == "mono8")
  {
    return 8;
  }
  else if (colorMode == "mono16")
  {
    return 16;
  }
  else
  {
    throw std::invalid_argument("Invalid color mode: " + colorMode);
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

int
DC1394::getDepth(unsigned bitsPerPixel) const
{
  int depth;
  if (bitsPerPixel == 8)
  {
    depth = IPL_DEPTH_8U;
  }
  else if (bitsPerPixel == 16)
  {
    depth = IPL_DEPTH_16U;
  }
  else
  {
    throw camera_parameter_error("Invalid image depth");
  }

  return depth;
}

void
DC1394::setFeatures()
{
  typedef boost::tuple<std::string, dc1394feature_t, std::string> FeatureTuple;

  std::vector<FeatureTuple > features;
  features += FeatureTuple(this->brightness, DC1394_FEATURE_BRIGHTNESS,
                           "brightness"),
              FeatureTuple(this->exposure, DC1394_FEATURE_EXPOSURE,
                           "exposure"),
              FeatureTuple(this->shutter, DC1394_FEATURE_SHUTTER,
                           "shutter"),
              FeatureTuple(this->gain, DC1394_FEATURE_GAIN,
                           "gain");

  FeatureTuple featureTuple;
  dc1394switch_t power;
  dc1394feature_mode_t featureMode;
  unsigned value;
  BOOST_FOREACH(featureTuple, features)
  {
    if (featureTuple.get<0>() == "untouched")
    {
      continue;
    }

    power = (featureTuple.get<0>() == "off") ? DC1394_OFF : DC1394_ON;
    featureMode = (featureTuple.get<0>() == "auto") ?
      DC1394_FEATURE_MODE_AUTO : DC1394_FEATURE_MODE_MANUAL;

    //If there are a unsigned integer inside feature value string
    if (power == DC1394_ON && featureMode == DC1394_FEATURE_MODE_MANUAL)
    {
      try{ value = boost::lexical_cast<unsigned>(featureTuple.get<0>());}
      catch (const boost::bad_lexical_cast& error)
      {
        throw open_camera_error("Invalid value of " + featureTuple.get<2>() +
                                " of dc1394 camera uid: " + this->uid);
      }

      if (dc1394_feature_set_value(this->camera, featureTuple.get<1>(), value)
          != DC1394_SUCCESS)
      {
        throw open_camera_error("Error on setting " + featureTuple.get<2>() +
                                " to dc1394 camera uid: " + this->uid);
      }
    }

    if (dc1394_feature_set_mode(this->camera, featureTuple.get<1>(),
                                featureMode)
        != DC1394_SUCCESS)
    {
      throw open_camera_error("Error on setting auto mode to " +
                              featureTuple.get<2>() +
                              " feature of dc1394 camera uid: " + this->uid);
    }

    if (dc1394_feature_set_power(this->camera, featureTuple.get<1>(), power)
        != DC1394_SUCCESS)
    {
      throw open_camera_error("Error on setting mode on to " +
                              featureTuple.get<2>() +
                              " feature of dc1394 camera uid: " + this->uid);
    }
  }


  if (this->whiteBalance.at(0) != "untouched")
  {
    try
    {
      if (this->whiteBalance.at(0) == "off")
      {
        dc1394_feature_set_power(this->camera, DC1394_FEATURE_WHITE_BALANCE,
                                 DC1394_OFF);
      }
      else
      {
        if (dc1394_feature_whitebalance_set_value(this->camera,
                                                  boost::lexical_cast<unsigned>
                                                    (this->whiteBalance.at(0)),
                                                  boost::lexical_cast<unsigned>
                                                    (this->whiteBalance.at(1)))
            != DC1394_SUCCESS)
        {
          throw open_camera_error("Error on setting " + featureTuple.get<2>() +
                                " to dc1394 camera uid: " + this->uid);
        }
      }
    }
    catch (const boost::bad_lexical_cast& error)
    {
      throw open_camera_error("Invalid white balance value to "
                              "dc1394 camera uid: "+ this->uid);
    }
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

  if (dc1394_video_set_mode(camera, this->videoMode))
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

  this->setFeatures();

  if (dc1394_video_set_transmission(this->camera, DC1394_ON))
  {
    throw open_camera_error("Could not start camera iso transmission");
  }
  this->grabFrameThread.reset(new boost::thread(boost::bind<void>
                                                (&DC1394::doWork,
                                                 this)));
}

void
DC1394::doWork()
{
  dc1394video_frame_t* videoFrame = NULL;
  IplImageWrapper tmpFrame(this->frameSize, this->depth, 3);
  IplImageWrapper frameBuffer[2];
  int frameIdx = 0;

  for (int i = 0; i < 2; ++i)
  {
    frameBuffer[i] = IplImageWrapper(this->frameSize, this->depth, 3);
  }

  while (true)
  {
    boost::this_thread::interruption_point();

    if (dc1394_capture_dequeue(this->camera,
                               DC1394_CAPTURE_POLICY_WAIT,
                               &videoFrame))
    {
      throw capture_image_error("Could not capture a frame");
    }

    // This don't work for images whith more than 1 byte per pixel
    if (this->bitsPerPixel == 8)
    {
      if (dc1394_bayer_decoding_8bit((uint8_t*)videoFrame->image,
                                     (uint8_t*)tmpFrame.data(),
                                     this->frameSize.width,
                                     this->frameSize.height,
                                     DC1394_COLOR_FILTER_RGGB,
                                     DC1394_BAYER_METHOD_SIMPLE))
      {
        throw capture_image_error("Error on bayer decoding");

      }
    }
    else if (this->bitsPerPixel == 16)
    {
      if (dc1394_bayer_decoding_16bit((uint16_t*)videoFrame->image,
                                      (uint16_t*)tmpFrame.data(),
                                      this->frameSize.width,
                                      this->frameSize.height,
                                      DC1394_COLOR_FILTER_RGGB,
                                      DC1394_BAYER_METHOD_SIMPLE,
                                      this->bitsPerPixel))
      {
        throw capture_image_error("Error on bayer decoding");
      }
    }

    fixChannelOrder(tmpFrame, frameBuffer[frameIdx]);
    updateCurrentFrame(frameBuffer[frameIdx]);

    // Release the buffer
    if (dc1394_capture_enqueue(this->camera, videoFrame))
    {
      throw capture_image_error("Error on returning a frame to ring buffer");
    }

    frameIdx = (frameIdx+1) % 2;
  }
}
