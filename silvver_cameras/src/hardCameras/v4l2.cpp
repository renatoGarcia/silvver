/* Copyright 2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#include "v4l2.hpp"

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <cstring>

#include <fcntl.h>
#include <stropts.h>
#include <sys/mman.h>

#include "../frame.hpp"
#include "../log.hpp"
#include "../exceptions.hpp"

using namespace boost::assign;

const V4L2::ColorFormatMap V4L2::COLOR_FORMAT = map_list_of
  ("rgb1", boost::make_tuple(V4L2_PIX_FMT_RGB332, ColorSpace::NONE))
  ("r444", boost::make_tuple(V4L2_PIX_FMT_RGB444, ColorSpace::NONE))
  ("rgbo", boost::make_tuple(V4L2_PIX_FMT_RGB555, ColorSpace::NONE))
  ("rgbp", boost::make_tuple(V4L2_PIX_FMT_RGB565, ColorSpace::NONE))
  ("rgbq", boost::make_tuple(V4L2_PIX_FMT_RGB555X, ColorSpace::NONE))
  ("rgbr", boost::make_tuple(V4L2_PIX_FMT_RGB565X, ColorSpace::NONE))
  ("bgr3", boost::make_tuple(V4L2_PIX_FMT_BGR24, ColorSpace::NONE))
  ("rgb3", boost::make_tuple(V4L2_PIX_FMT_RGB24, ColorSpace::RGB8))
  ("bgr4", boost::make_tuple(V4L2_PIX_FMT_BGR32, ColorSpace::NONE))
  ("rgb4", boost::make_tuple(V4L2_PIX_FMT_RGB32, ColorSpace::NONE))
  ("y444", boost::make_tuple(V4L2_PIX_FMT_YUV444, ColorSpace::NONE))
  ("yuvo", boost::make_tuple(V4L2_PIX_FMT_YUV555, ColorSpace::NONE))
  ("yuvp", boost::make_tuple(V4L2_PIX_FMT_YUV565, ColorSpace::NONE))
  ("yuv4", boost::make_tuple(V4L2_PIX_FMT_YUV32, ColorSpace::NONE))
  ("grey", boost::make_tuple(V4L2_PIX_FMT_GREY, ColorSpace::MONO8))
  ("y16" , boost::make_tuple(V4L2_PIX_FMT_Y16, ColorSpace::NONE))
  ("yuyv", boost::make_tuple(V4L2_PIX_FMT_YUYV, ColorSpace::YUYV))
  ("uyvy", boost::make_tuple(V4L2_PIX_FMT_UYVY, ColorSpace::UYVY))
  ("y41p", boost::make_tuple(V4L2_PIX_FMT_Y41P, ColorSpace::NONE))
  ("yv12", boost::make_tuple(V4L2_PIX_FMT_YVU420, ColorSpace::NONE))
  ("yu12", boost::make_tuple(V4L2_PIX_FMT_YUV420, ColorSpace::NONE))
  ("yvu9", boost::make_tuple(V4L2_PIX_FMT_YVU410, ColorSpace::NONE))
  ("yuv9", boost::make_tuple(V4L2_PIX_FMT_YUV410, ColorSpace::NONE))
  ("422p", boost::make_tuple(V4L2_PIX_FMT_YUV422P, ColorSpace::NONE))
  ("411p", boost::make_tuple(V4L2_PIX_FMT_YUV411P, ColorSpace::NONE))
  ("nv12", boost::make_tuple(V4L2_PIX_FMT_NV12, ColorSpace::NONE))
  ("nv21", boost::make_tuple(V4L2_PIX_FMT_NV21, ColorSpace::NONE))
  ("mjpg", boost::make_tuple(V4L2_PIX_FMT_MJPEG, ColorSpace::NONE));

V4L2::V4L2(const scene::V4l2& config)
  :HardCamera(config, IPL_DEPTH_8U)
  ,uid(config.uid)
  ,cameraFd(open(this->findDevice().c_str(), O_RDWR))
  ,grabFrameThread()
  ,colorConverter(V4L2::createColorConverter(config))
{
  // Check if the device is compatible with v4l2
  struct v4l2_capability capability;
  if (ioctl(this->cameraFd, VIDIOC_QUERYCAP, &capability))
  {
    throw open_camera_error()
      << info_what("Device not compatible with v4l2 especification")
      << info_cameraUid(boost::lexical_cast<std::string>(this->uid));
  }

  // Check if the device is a video capture with streaming capability.
  if (!((capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) &&
        (capability.capabilities & V4L2_CAP_STREAMING)))
  {
    throw open_camera_error()
      << info_what("Device is not a video capture with streaming capabilities")
      << info_cameraUid(boost::lexical_cast<std::string>(this->uid));
  }

  try
  {
    setFormat(config);
    setFeatures(config);
  }
  catch (camera_parameter_error& e)
  {
    throw e << info_cameraUid(boost::lexical_cast<std::string>(this->uid));
  }

  struct v4l2_requestbuffers requestbuffers;
  requestbuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  requestbuffers.count = V4L2::N_BUFFERS;
  requestbuffers.memory = V4L2_MEMORY_MMAP;
  if (ioctl(this->cameraFd, VIDIOC_REQBUFS, &requestbuffers))
  {
    throw open_camera_error()
      << info_what("Could not request buffers")
      << info_cameraUid(boost::lexical_cast<std::string>(this->uid));
  }
  if (requestbuffers.count != (uint)V4L2::N_BUFFERS)
  {
    throw open_camera_error()
      << info_what("Could not request all buffers")
      << info_cameraUid(boost::lexical_cast<std::string>(this->uid));
  }

  for (int i = 0; i < V4L2::N_BUFFERS; i++)
  {
    struct v4l2_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));

    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory = V4L2_MEMORY_MMAP;
    buffer.index = i;

    if (ioctl(this->cameraFd, VIDIOC_QUERYBUF, &buffer))
    {
      throw open_camera_error()
        << info_what("Coud not query the status of a buffer")
        << info_cameraUid(boost::lexical_cast<std::string>(this->uid));
    }

    this->buffers[i].length = buffer.length;
    this->buffers[i].start = (uint8_t*)mmap(NULL, buffer.length,
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED,
                                            this->cameraFd, buffer.m.offset);

    if (MAP_FAILED == this->buffers[i].start)
    {
      throw open_camera_error()
        << info_what("Coud not map the memory of buffer")
        << info_cameraUid(boost::lexical_cast<std::string>(this->uid));
    }
  }

  for (int i = 0; i < V4L2::N_BUFFERS; ++i)
  {
    struct v4l2_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));

    buffer.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory  = V4L2_MEMORY_MMAP;
    buffer.index   = i;

    if (ioctl(this->cameraFd, VIDIOC_QBUF, &buffer))
    {
      throw open_camera_error()
        << info_what("Coud not enqueue a buffer")
        << info_cameraUid(boost::lexical_cast<std::string>(this->uid));
    }
  }

  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(this->cameraFd, VIDIOC_STREAMON, &type))
  {
    throw open_camera_error()
      << info_what("Failed when starting streaming")
      << info_cameraUid(boost::lexical_cast<std::string>(this->uid));
  }

  this->grabFrameThread.reset(new boost::thread(&V4L2::doWork, this));
}

V4L2::~V4L2() throw()
{
  if (this->grabFrameThread)
  {
    this->grabFrameThread->interrupt();
    this->grabFrameThread->join();
  }

  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(this->cameraFd, VIDIOC_STREAMOFF, &type))
  {
    message(LogLevel::ERROR)
      << ts_output::lock
      << "Failed to stop the streaming of v4l2 camera uid "
      << this->uid << std::endl
      << ts_output::unlock;
  }

  for (int i = 0; i < V4L2::N_BUFFERS; ++i)
    munmap(this->buffers[i].start, this->buffers[i].length);

  close(this->cameraFd);
}

std::string
V4L2::findDevice() const
{
  const std::string strUid = boost::lexical_cast<std::string>(this->uid);

  boost::array<std::string, 1> possibleDevices;
  possibleDevices.at(0) = "/dev/video" + strUid;

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
    << info_cameraUid(boost::lexical_cast<std::string>(this->uid));
}

ColorConverter
V4L2::createColorConverter(const scene::V4l2& config)
{
  try
  {
    return ColorConverter(V4L2::COLOR_FORMAT.find(config.colorMode)->second.get<1>(),
                          config.resolution.at(0), config.resolution.at(1));
  }
  catch (silvver_cameras_exception& e)
  {
    throw e << info_cameraUid(boost::lexical_cast<std::string>(config.uid))
            << info_colorMode(config.colorMode);
  }
}

void
V4L2::setFeatures(const scene::V4l2& config)
{
  // First field is control id, the second is control value, and the third
  // a string representation of control name
  typedef boost::tuple<__u32, boost::optional<int>, std::string > IntFeature;
  typedef boost::tuple<__u32, boost::optional<bool>, std::string> BoolFeature;

  std::vector<IntFeature> intFeatures;
  intFeatures += IntFeature(V4L2_CID_BRIGHTNESS, config.brightness,
                            "brightness"),
                 IntFeature(V4L2_CID_CONTRAST, config.contrast,
                            "contrast"),
                 IntFeature(V4L2_CID_SATURATION, config.saturation,
                            "saturation"),
                 IntFeature(V4L2_CID_HUE, config.hue,
                            "hue"),
                 IntFeature(V4L2_CID_RED_BALANCE, config.redBalance,
                            "red balance"),
                 IntFeature(V4L2_CID_BLUE_BALANCE, config.blueBalance,
                            "blue balance"),
                 IntFeature(V4L2_CID_GAMMA, config.gamma,
                            "gamma"),
                 IntFeature(V4L2_CID_EXPOSURE, config.exposure,
                            "exposure"),
                 IntFeature(V4L2_CID_GAIN, config.gain,
                            "gain"),
                 IntFeature(V4L2_CID_POWER_LINE_FREQUENCY,
                            config.powerLineFrequency,
                            "power line frequency"),
                 IntFeature(V4L2_CID_WHITE_BALANCE_TEMPERATURE,
                            config.whiteBalanceTemperature,
                            "white balance temperature"),
                 IntFeature(V4L2_CID_SHARPNESS, config.sharpness,
                            "sharpness"),
                 IntFeature(V4L2_CID_BACKLIGHT_COMPENSATION,
                            config.backlightCompensation,
                            "backlight compensation");

  //-------------- Convert the optional bool to optional int
  std::vector<BoolFeature> boolFeatures;
  boolFeatures += BoolFeature(V4L2_CID_AUTO_WHITE_BALANCE,
                              config.autoWhiteBalance,
                              "auto white balance"),
                  BoolFeature(V4L2_CID_AUTOGAIN, config.autogain,
                              "auto gain"),
                  BoolFeature(V4L2_CID_HFLIP, config.horizontalFlip,
                              "horizontal flip"),
                  BoolFeature(V4L2_CID_VFLIP, config.verticalFlip,
                              "vertical flip"),
                  BoolFeature(V4L2_CID_HUE_AUTO, config.hueAuto,
                              "hue auto");
  BoolFeature boolFeature;
  BOOST_FOREACH(boolFeature, boolFeatures)
  {
    IntFeature convertedFeature;
    convertedFeature.get<0>() = boolFeature.get<0>();
    convertedFeature.get<2>() = boolFeature.get<2>();
    if(boolFeature.get<1>())
    {
      convertedFeature.get<1>() = int(*boolFeature.get<1>());
    }
    intFeatures += convertedFeature;
  }

  //------------------------------------- Set the features
  struct v4l2_queryctrl queryctrl;
  struct v4l2_control control;

  IntFeature feature;
  BOOST_FOREACH(feature, intFeatures)
  {
    std::string featureName(feature.get<2>());
    // If feature don't given
    if (!feature.get<1>())
    {
      continue;
    }

    memset(&queryctrl, 0, sizeof(queryctrl));
    queryctrl.id = feature.get<0>();

    // Query about the current feature
    if (ioctl(this->cameraFd, VIDIOC_QUERYCTRL, &queryctrl) == -1)
    {
      if (errno != EINVAL) // If an unknown error
      {
        throw camera_parameter_error()
          << info_what("Error querying about feature")
          << info_featureName(featureName);
      }
      else // The current feature is not supported
      {
        throw camera_parameter_error()
          << info_what("Feature not supported")
          << info_featureName(featureName);
      }
    }
    // If the support to current feature is disabled (not supported)
    else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
    {
      throw camera_parameter_error()
        << info_what( "feature not supported")
        << info_featureName(featureName);
    }
    else // There are support to this feature
    {
      memset(&control, 0, sizeof (control));
      control.id = feature.get<0>();
      control.value = *feature.get<1>();

      if (control.value<queryctrl.minimum || control.value>queryctrl.maximum)
      {
        throw camera_parameter_error()
          << info_what("Feature value out of rage")
          << info_featureName(featureName)
          << info_featureRange(boost::lexical_cast<std::string>(queryctrl.minimum)
                               + '-' +
                               boost::lexical_cast<std::string>(queryctrl.maximum));
      }

      if (-1 == ioctl(this->cameraFd, VIDIOC_S_CTRL, &control))
      {
        if (errno == EBUSY)
        {
          throw camera_parameter_error()
            << info_what("Error setting feature")
            << info_featureName(featureName);
        }
        else
        {
          throw camera_parameter_error()
            << info_what("Error setting feature")
            << info_featureName(featureName);
        }
      }
    }
  }
}

void
V4L2::setFormat(const scene::V4l2& config)
{
  struct v4l2_format imgFormat;
  imgFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(this->cameraFd, VIDIOC_G_FMT, &imgFormat))
  {
    throw open_camera_error()
      << info_what("Cannot get the camera images format");
  }

  IterColorFormatMap pixelFormat = V4L2::COLOR_FORMAT.find(config.colorMode);
  if (pixelFormat != V4L2::COLOR_FORMAT.end())
  {
    imgFormat.fmt.pix.width = config.resolution.at(0);
    imgFormat.fmt.pix.height = config.resolution.at(1);
    imgFormat.fmt.pix.pixelformat = pixelFormat->second.get<0>();

    if (ioctl(this->cameraFd, VIDIOC_S_FMT, &imgFormat))
    {
      throw open_camera_error()
        << info_what("Could not set resolution and color mode together")
        << info_resolution(config.resolution)
        << info_colorMode(config.colorMode);
    }
    if (imgFormat.fmt.pix.width != config.resolution.at(0) ||
        imgFormat.fmt.pix.height != config.resolution.at(1))
    {
      throw open_camera_error()
        << info_what("Could not set resolution")
        << info_resolution(config.resolution);
    }
  }
  else //Color mode is not in V4L2::COLOR_FORMAT
  {
    throw invalid_argument()
      << info_what("Invalid color mode")
      << info_resolution(config.resolution)
      << info_colorMode(config.colorMode);
  }
}

void
V4L2::doWork()
{
  struct v4l2_buffer buf;
  memset(&buf, 0, sizeof(buf));
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;

  boost::shared_ptr<Frame> frameBuffer[2];
  int depth = IPL_DEPTH_8U;
  frameBuffer[0].reset(new Frame(this->frameSize, depth, 3));
  frameBuffer[1].reset(new Frame(this->frameSize, depth, 3));
  int frameIdx = 0;

  while (true)
  {
    boost::this_thread::interruption_point();

    if (ioctl(this->cameraFd, VIDIOC_DQBUF, &buf))
    {
      message(LogLevel::ERROR)
        << ts_output::lock
        << "Error dequeuing buffer on v4l2 camera uid: "
        << this->uid << std::endl
        << ts_output::unlock;
    }

    frameBuffer[frameIdx]->timestamp = buf.timestamp.tv_sec * 1000000 +
                                       buf.timestamp.tv_usec;
    this->colorConverter((uint8_t*)buffers[buf.index].start,
                         *frameBuffer[frameIdx]);

    updateCurrentFrame(frameBuffer[frameIdx]);

    if (ioctl(this->cameraFd, VIDIOC_QBUF, &buf))
    {
      message(LogLevel::ERROR)
        << ts_output::lock
        << "Error enqueuing buffer on v4l2 camera uid: "
        << this->uid << std::endl
        << ts_output::unlock;
    }

    frameIdx = (frameIdx+1) % 2;
  }
}
