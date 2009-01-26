#include "hardCamera.hpp"

HardCamera::HardCamera(std::string UID,
                       Resolution resolution,
                       FrameRate frameRate)
  :UID(UID)
  ,frameRate(frameRate)
{
  if(resolution == HardCamera::RESOLUTION_640x480)
  {
    this->frameWidth = 640;
    this->frameHeight = 480;
    this->frameSize = 640 * 480;
  }
}

HardCamera::~HardCamera()
{}

void
HardCamera::createIplImage(IplImage* &iplImage) const
{
  iplImage = cvCreateImage(cvSize(this->frameWidth, this->frameHeight),
                           IPL_DEPTH_8U,
                           3);
}
