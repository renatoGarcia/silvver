#include "pseudoCamera.hpp"

#include <boost/thread/thread.hpp>

namespace bfs = boost::filesystem;
namespace bpt = boost::posix_time;

PseudoCamera::PseudoCamera(const scene::Camera& config)
  :HardCamera(config)
  ,path(config.imagesPath)
  ,delay((static_cast<long>((1.0 / this->frameRate) * 1.0e3)))
{}

PseudoCamera::~PseudoCamera()
{}

void
PseudoCamera::initialize()
{
  if(!bfs::is_directory(this->path))
  {
    throw camera_parameter_error(this->path.directory_string() +
                                 " is not a directory");
  }

  this->dirIterator = bfs::directory_iterator(this->path);
}

void
PseudoCamera::saveFrame()
{}

void
PseudoCamera::captureFrame(IplImage** iplImage)
{
  boost::mutex::scoped_lock lock(this->mutexCaptureFrame);

  bool imageLoaded = false;
  while(!imageLoaded)
  {
    if(this->dirIterator == this->endIterator)
    {
      boost::this_thread::sleep(this->delay);
      throw capture_image_error("PseudoCamera: all images already read");
    }

    if(bfs::is_directory(dirIterator->status()))
    {
      this->dirIterator++;
      continue;
    }

    cvReleaseImage(iplImage);
    *iplImage = cvLoadImage(dirIterator->path().file_string().c_str(),
                            CV_LOAD_IMAGE_COLOR);
    if(*iplImage == NULL)
    {
      this->dirIterator++;
      throw capture_image_error("Error loading image: " +
                                dirIterator->path().file_string());
    }
    imageLoaded = true;
    this->dirIterator++;
  }

  boost::this_thread::sleep(this->delay);
}
