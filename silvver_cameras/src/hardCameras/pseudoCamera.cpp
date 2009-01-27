#include "pseudoCamera.hpp"
#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/mutex.hpp>

extern boost::mutex mutexCout;

PseudoCamera::PseudoCamera(const std::string& uid,
                           const boost::array<unsigned, 2>& resolution,
                           float frameRate,
                           const std::string& imagesPath)
  :HardCamera(uid, resolution, frameRate)
  ,PATH(imagesPath)
{}

PseudoCamera::~PseudoCamera()
{}

void
PseudoCamera::initialize()
{
  if(!bfs::is_directory(this->PATH))
  {
    throw camera_parameter_error("Path for images in PseudoCamera don't is a directory");
  }

  this->delay = (unsigned)((1.0/this->frameRate)*1.0e9);

  this->dirIterator = bfs::directory_iterator(this->PATH);
}

void PseudoCamera::saveFrame()
{}

void PseudoCamera::captureFrame(IplImage* &iplImage)
{
  boost::mutex::scoped_lock lock(this->mutexCaptureFrame);

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

    cvReleaseImage(&iplImage);
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
