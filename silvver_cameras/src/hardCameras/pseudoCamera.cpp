#include "pseudoCamera.hpp"
#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/mutex.hpp>


extern boost::mutex mutexCout;

PseudoCamera::PseudoCamera(uint64 UID,
                           FrameRate frameRate,
                           Resolution resolution,
                           std::string imagesPath)
  :HardCamera(UID, resolution, frameRate)
  ,PATH(imagesPath)
{}

PseudoCamera::~PseudoCamera()
{}

void
PseudoCamera::initialize()
{
  if(!bfs::is_directory(this->PATH))
  {
    throw OpenCameraFailed("Path for images in PseudoCamera don't is a directory");
  }

  switch(this->frameRate)
  {
  case HardCamera::FR_7_5:
    this->delay = (unsigned)((1.0/7.5)*1.0e9);
    break;
  case HardCamera::FR_15:
    this->delay = (unsigned)((1.0/15.0)*1.0e9);
    break;
  case HardCamera::FR_30:
    this->delay = (unsigned)((1.0/30.0)*1.0e9);
    break;
  default:
    throw OpenCameraFailed("Frame Rate don't allowed");
  }

  this->dirIterator = bfs::directory_iterator(this->PATH);

 }

void PseudoCamera::saveFrame()
{}

void PseudoCamera::captureFrame(IplImage* &iplImage)
{
  boost::xtime xt;
  boost::xtime_get(&xt, boost::TIME_UTC);
  xt.nsec += this->delay;

  bool imageLoaded = false;
  while(!imageLoaded)
  {
    if(this->dirIterator == this->endIterator)
    {
      return;
    }

    if(bfs::is_directory(dirIterator->status()))
    {
      this->dirIterator++;
      continue;
    }

    cvReleaseImage( &iplImage );
    iplImage = cvLoadImage(dirIterator->path().file_string().c_str(),
                           CV_LOAD_IMAGE_COLOR);
    if(iplImage == NULL)
    {
      std::cerr << "Error on load image " << dirIterator->path() << std::endl;
      this->dirIterator++;
      continue;
    }
    imageLoaded = true;
    this->dirIterator++;
  }

  boost::thread::sleep(xt);

}

