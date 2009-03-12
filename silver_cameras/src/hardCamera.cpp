#include "hardCamera.hpp"

HardCamera::HardCamera(const std::string& uid,
                       const boost::array<unsigned, 2>& resolution,
                       float frameRate)
  :uid(uid)
  ,frameSize(resolution.at(0) * resolution.at(1))
  ,frameWidth(resolution.at(0))
  ,frameHeight(resolution.at(1))
  ,frameRate(frameRate)
{}

HardCamera::~HardCamera()
{}

void
HardCamera::createIplImage(IplImage* &iplImage) const
{
  iplImage = cvCreateImage(cvSize(this->frameWidth, this->frameHeight),
                           IPL_DEPTH_8U,
                           3);
}
