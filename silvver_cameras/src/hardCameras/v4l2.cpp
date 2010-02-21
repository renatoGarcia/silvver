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

#include <iostream>
#include <boost/assign/std/vector.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <stropts.h>
#include <sys/mman.h>

#include "../iplImageWrapper.hpp"

using namespace boost::assign;

// color conversion functions from Bart Nabbe.
// corrected by Damien: bad coeficients in YUV2RGB
#define YUV2RGB(y, u, v, r, g, b) {\
  r = y + ((v*1436) >> 10);\
  g = y - ((u*352 + v*731) >> 10);\
  b = y + ((u*1814) >> 10);\
  r = r < 0 ? 0 : r;\
  g = g < 0 ? 0 : g;\
  b = b < 0 ? 0 : b;\
  r = r > 255 ? 255 : r;\
  g = g > 255 ? 255 : g;\
  b = b > 255 ? 255 : b; }

typedef enum {
    BYTE_ORDER_UYVY,
    BYTE_ORDER_YUYV
} byte_order_t;

void
YUV422_to_BGR8(uint8_t *src, uint8_t *dest,
               uint32_t width, uint32_t height, uint32_t byte_order)
{
    register int i = ((width*height) << 1)-1;
    register int j = (width*height) + ( (width*height) << 1 ) -1;
    register int y0, y1, u, v;
    register int r, g, b;


    switch (byte_order) {
    case BYTE_ORDER_YUYV:
        while (i >= 0) {
            v  = (uint8_t) src[i--] -128;
            y1 = (uint8_t) src[i--];
            u  = (uint8_t) src[i--] -128;
            y0  = (uint8_t) src[i--];
            YUV2RGB (y1, u, v, r, g, b);
            dest[j--] = r;
            dest[j--] = g;
            dest[j--] = b;
            YUV2RGB (y0, u, v, r, g, b);
            dest[j--] = r;
            dest[j--] = g;
            dest[j--] = b;
        }
        return;// LIBDC1394_SUCCESS;
    case BYTE_ORDER_UYVY:
        while (i >= 0) {
            y1 = (uint8_t) src[i--];
            v  = (uint8_t) src[i--] - 128;
            y0 = (uint8_t) src[i--];
            u  = (uint8_t) src[i--] - 128;
            YUV2RGB (y1, u, v, r, g, b);
            dest[j--] = r;
            dest[j--] = g;
            dest[j--] = b;
            YUV2RGB (y0, u, v, r, g, b);
            dest[j--] = r;
            dest[j--] = g;
            dest[j--] = b;
        }
        return;// LIBDC1394_SUCCESS;
    default:
      return;// LIBDC1394_INVALID_BYTE_ORDER;
    }
}


V4L2::V4L2(const scene::V4l2& config)
  :HardCamera(config, IPL_DEPTH_8U, 3)//getBitsPerPixel(config.colorMode))
  ,uid(config.uid)
  ,cameraPath("/dev/video0")
  ,cameraFd(open(this->cameraPath.c_str(), O_RDWR))
{
  struct v4l2_capability capability;
  if (ioctl(this->cameraFd, VIDIOC_QUERYCAP, &capability))
  {
    std::cout << "The device in " + this->cameraPath + " is not compatible "
                 "with v4l2 especification"
              << std::endl;
  }

  // Check if the device is a video capture with streaming capability.
  if (!((capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) &&
        (capability.capabilities & V4L2_CAP_STREAMING)))
  {
    std::cout << "The device in " + this->cameraPath + " is not a video "
                 "capture with streaming capabilities."
              << std::endl;
  }

  struct v4l2_format imgFormat;
  imgFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(this->cameraFd, VIDIOC_G_FMT, &imgFormat))
  {
    throw open_camera_error("Cannot get the images format from V4l2 camera "
                            " in " + this->cameraPath);
  }

  // struct v4l2_standard camStandard;
  // camStandard.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  setFeatures(config);

  struct v4l2_requestbuffers requestbuffers;
  requestbuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  requestbuffers.count = V4L2::N_BUFFERS;
  requestbuffers.memory = V4L2_MEMORY_MMAP;
  if (ioctl(this->cameraFd, VIDIOC_REQBUFS, &requestbuffers))
  {
    std::cout << "Could not request buffers to the device in " +
                 this->cameraPath + ". Maybe it is being used by other application."
              << std::endl;
  }
  if (requestbuffers.count != (uint)V4L2::N_BUFFERS)
  {
    std::cout << "Fudeu!" << std::endl;
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
      std::cout << "Fudeu!" << std::endl;
    }

    this->buffers[i].length = buffer.length;
    this->buffers[i].start = (uint8_t*)mmap(NULL, buffer.length,
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED,
                                            this->cameraFd, buffer.m.offset);

    if (MAP_FAILED == this->buffers[i].start)
    {
      // /* If you do not exit here you should unmap() and free()
      //    the buffers mapped so far. */
      // perror("mmap");
      // exit(EXIT_FAILURE);
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
      std::cout << "Fudeu!" << std::endl;
    }
  }

  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(this->cameraFd, VIDIOC_STREAMON, &type))
  {
    std::cout << "Fudeu!" << std::endl;
  }

  this->grabFrameThread.reset(new boost::thread(&V4L2::doWork, this));
}

V4L2::~V4L2()
{
  if (this->grabFrameThread)
  {
    this->grabFrameThread->interrupt();
    this->grabFrameThread->join();
  }

  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(this->cameraFd, VIDIOC_STREAMOFF, &type))
  {
    std::cout << "Fudeu!" << std::endl;
  }

  for (int i = 0; i < V4L2::N_BUFFERS; ++i)
    munmap(this->buffers[i].start, this->buffers[i].length);

  close(this->cameraFd);
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

    if (ioctl(this->cameraFd, VIDIOC_QUERYCTRL, &queryctrl) == -1)
    {
      if (errno != EINVAL)
      {
        throw camera_parameter_error
          ("Error when querying about the " + featureName + " feature of " +
           "v4l2 camera with id " + boost::lexical_cast<std::string>(uid));
      }
      else
      {
        throw camera_parameter_error
          (featureName + " feature is not supported by v4l2 camera uid " +
           boost::lexical_cast<std::string>(uid));
      }
    }
    else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
    {
      throw camera_parameter_error
        (featureName + " feature is not supported by v4l2 camera uid " +
         boost::lexical_cast<std::string>(uid));
    }
    else
    {
      memset(&control, 0, sizeof (control));
      control.id = feature.get<0>();
      control.value = *feature.get<1>();

      if (control.value<queryctrl.minimum || control.value>queryctrl.maximum)
      {
        throw camera_parameter_error
          ("The " + featureName + " feature of v4l2 camera with id " +
           boost::lexical_cast<std::string>(uid) + " must be between " +
           boost::lexical_cast<std::string>(queryctrl.minimum) + " and " +
           boost::lexical_cast<std::string>(queryctrl.maximum));
      }

      if (-1 == ioctl(this->cameraFd, VIDIOC_S_CTRL, &control))
      {
        if (errno == EBUSY)
        {
          throw camera_parameter_error
            ("Error when setting the " + featureName + " feature of v4l2 " +
             " camera uid " + boost::lexical_cast<std::string>(uid) +
             ". Possibly this is because another applications took over "
             "control of the device function this control belongs to.");
        }
        else
        {
          throw camera_parameter_error
            ("Error when setting the " + featureName + " feature of v4l2 " +
             " camera uid " + boost::lexical_cast<std::string>(uid));
        }
      }
    }
  }
}

// std::string
// V4L2::findVideo1394Device(unsigned cardNumber)
// {
//   std::string strCardNumber(boost::lexical_cast<std::string>(cardNumber));

//   boost::array<std::string, 3> possibleDevices;
//   possibleDevices.at(0) = "/dev/video1394/" + strCardNumber;
//   possibleDevices.at(1) = "/dev/video1394-" + strCardNumber;
//   possibleDevices.at(2) = "/dev/video1394" + strCardNumber;

//   struct stat statbuf;
//   std::string device;
//   BOOST_FOREACH (device, possibleDevices)
//   {
//     if (stat(device.c_str(), &statbuf) == 0 && S_ISCHR(statbuf.st_mode))
//     {
//       return device;
//     }
//   }

//   // If here, didn't found camera devide
//   throw open_camera_error("Don't found the device of camera with uid " +
//                           this->uid);
// }

void
V4L2::initialize()
{
}

void
V4L2::doWork()
{
  struct v4l2_buffer buf;
  memset(&buf, 0, sizeof(buf));
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;

  boost::shared_ptr<IplImageWrapper> frameBuffer[2];
  int depth = IPL_DEPTH_8U;
  frameBuffer[0].reset(new IplImageWrapper(this->frameSize, depth, 3));
  frameBuffer[1].reset(new IplImageWrapper(this->frameSize, depth, 3));
  int frameIdx = 0;

  while (true)
  {
    boost::this_thread::interruption_point();

    if (ioctl(this->cameraFd, VIDIOC_DQBUF, &buf))
    {
      // printf("VIDIOC_DQBUF");
    }

    YUV422_to_BGR8((uint8_t*)buffers[buf.index].start,
                   (uint8_t*)frameBuffer[frameIdx]->data(),
                   640, 480, BYTE_ORDER_YUYV);

    updateCurrentFrame(frameBuffer[frameIdx]);

    if (ioctl(this->cameraFd, VIDIOC_QBUF, &buf))
    {
      // printf("VIDIOC_QBUF");
    }

    frameIdx = (frameIdx+1) % 2;
  }
}
