#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "v4l2.hpp"

#include <stropts.h>
#include <sys/mman.h>

#include "../iplImageWrapper.hpp"

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
  :HardCamera(config, 8)//getBitsPerPixel(config.colorMode))
  ,cameraPath("/dev/video0")
{}

V4L2::~V4L2()
{
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
  this->cameraFd = open(this->cameraPath.c_str(), O_RDWR);

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
  //-----------------------------------------------------------------------
  struct v4l2_querymenu querymenu;
  struct v4l2_queryctrl queryctrl;

  memset (&queryctrl, 0, sizeof (queryctrl));

  for (queryctrl.id = V4L2_CID_BASE;
       queryctrl.id < V4L2_CID_LASTP1;
       queryctrl.id++)
  {
    if (ioctl (this->cameraFd, VIDIOC_QUERYCTRL, &queryctrl))
    {
      std::cout << "biziu" << std::endl;
      // if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
      //   continue;

      // std::cout << "Control " << queryctrl.name << std::endl;

      //           if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
      //                   enumerate_menu ();
      //   } else {
      //           if (errno == EINVAL)
      //                   continue;

      //           perror ("VIDIOC_QUERYCTRL");
      //           exit (EXIT_FAILURE);
      //   }
    }

// for (queryctrl.id = V4L2_CID_PRIVATE_BASE;;
//      queryctrl.id++) {
//         if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
//                 if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
//                         continue;

//                 printf ("Control %s\n", queryctrl.name);

//                 if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
//                         enumerate_menu ();
//         } else {
//                 if (errno == EINVAL)
//                         break;

//                 perror ("VIDIOC_QUERYCTRL");
//                 exit (EXIT_FAILURE);
//         }
  }

  //-----------------------------------------------------------------------


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
  if (requestbuffers.count != V4L2::N_BUFFERS)
  {
    std::cout << "Fudeu!" << std::endl;
  }

  // this->buffers = new V4L2::Buffers[V4L2::N_BUFFERS];
  for (int i = 0; i < V4L2::N_BUFFERS; i++)
  {
    struct v4l2_buffer buffer;
    this->clear(buffer);

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
    this->clear(buffer);

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

void
V4L2::doWork()
{
  struct v4l2_buffer buf;
  this->clear(buf);
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;

  IplImageWrapper frameBuffer[2];
  int depth = IPL_DEPTH_8U;
  frameBuffer[0] = IplImageWrapper(this->frameSize, depth, 3);
  frameBuffer[1] = IplImageWrapper(this->frameSize, depth, 3);
  int frameIdx = 0;

  while (true)
  {
    boost::this_thread::interruption_point();

    if (ioctl(this->cameraFd, VIDIOC_DQBUF, &buf))
    {
      // printf("VIDIOC_DQBUF");
    }

    YUV422_to_BGR8((uint8_t*)buffers[buf.index].start,
                   (uint8_t*)frameBuffer[frameIdx].data(),
                   640, 480, BYTE_ORDER_YUYV);

    updateCurrentFrame(frameBuffer[frameIdx]);

    if (ioctl(this->cameraFd, VIDIOC_QBUF, &buf))
    {
      // printf("VIDIOC_QBUF");
    }

    frameIdx = (frameIdx+1) % 2;
  }
}
