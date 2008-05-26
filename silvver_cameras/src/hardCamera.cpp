#include "hardCamera.hpp"

HardCamera::HardCamera(Resolution resolution, FrameRate frameRate)
  :frameRate(frameRate)
{
  if(resolution == HardCamera::RESOLUTION_640x480)
  {
    this->frameWidth = 640;
    this->frameHeight = 480;
    this->frameSize = 640 * 480;
  }
}

void
HardCamera::createIplImage(IplImage* &iplImage)
{
  iplImage = cvCreateImage(cvSize(this->frameWidth, this->frameHeight),
                           IPL_DEPTH_8U, 3);
}
