#include "abstractCamera.hpp"

#include <cstddef>

#include <boost/ref.hpp>

#include "hardCameraFactory.hpp"
#include "tsPrint.hpp"

AbstractCamera::AbstractCamera(const scene::Camera& cameraConfig,
                               boost::shared_ptr<Connection> connection)
  :currentFrame(NULL)
  ,serverConnection(connection)
  ,stopping(false)
  ,hardCamera(HardCameraFactory::create(cameraConfig))
  ,frameCounter(0)
  ,frameRate(0)
{
  if (const scene::MatrixHomography* const matrixHomography =
      boost::get<const scene::MatrixHomography>(&cameraConfig.homography))
  {
    this->homography.reset(new Matrix(*matrixHomography));
  }
  else if (const scene::LutHomography* const lutHomography =
           boost::get<const scene::LutHomography>(&cameraConfig.homography))
  {
    this->homography.reset(new Lut(*lutHomography, cameraConfig.resolution));
  }

  this->hardCamera->createIplImage(this->currentFrame);
}

AbstractCamera::~AbstractCamera()
{
  cvReleaseImage(&currentFrame);
}

void
AbstractCamera::run()
{
  this->runThread.reset(new boost::thread(boost::ref(*this)));
}

void
AbstractCamera::stop()
{
  this->stopping = true;
  this->runThread->join();
}

void
AbstractCamera::updateFrame()
{
  try
  {
    hardCamera->captureFrame(this->currentFrame);
  }
  catch(const HardCamera::capture_image_error& exception)
  {
    {PRINT_LOCK;
      std::cerr << exception.what() << std::endl;}
    return;
  }

//   this->frameCounter++;
//   if(this->frameCounter >= 30)
//   {
//     this->frameRate = float( this->frameCounter/timer->ElapsedTime() );
//     quadros = 0;
//     timer->Start();
//   }
}

void
AbstractCamera::localize(silver::Position &position) const
{
  this->homography->transform2word(position);
}
