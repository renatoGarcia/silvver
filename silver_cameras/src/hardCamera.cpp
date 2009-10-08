#include "hardCamera.hpp"

HardCamera::HardCamera(const scene::Hardware& config)
  :frameSize(config.resolution.at(0) * config.resolution.at(1))
  ,frameWidth(config.resolution.at(0))
  ,frameHeight(config.resolution.at(1))
  ,mapx(cvCreateImage(cvSize(this->frameWidth, this->frameHeight),
                      IPL_DEPTH_32F, 1))
  ,mapy(cvCreateImage(cvSize(this->frameWidth, this->frameHeight),
                      IPL_DEPTH_32F, 1))
{
  CvMat* intrinsic = cvCreateMat(3, 3, CV_32FC1);
  CvMat* distortion = cvCreateMat(5, 1, CV_32FC1);

  CV_MAT_ELEM(*intrinsic, float, 0, 0) = config.focalLength.at(0);
  CV_MAT_ELEM(*intrinsic, float, 0, 1) = 0;
  CV_MAT_ELEM(*intrinsic, float, 0, 2) = config.principalPoint.at(0);
  CV_MAT_ELEM(*intrinsic, float, 1, 0) = 0;
  CV_MAT_ELEM(*intrinsic, float, 1, 1) = config.focalLength.at(1);
  CV_MAT_ELEM(*intrinsic, float, 1, 2) = config.principalPoint.at(1);
  CV_MAT_ELEM(*intrinsic, float, 2, 0) = 0;
  CV_MAT_ELEM(*intrinsic, float, 2, 1) = 0;
  CV_MAT_ELEM(*intrinsic, float, 2, 2) = 1;

  CV_MAT_ELEM(*distortion, float, 0, 0) = config.radialCoef.at(0);
  CV_MAT_ELEM(*distortion, float, 1, 0) = config.radialCoef.at(1);
  CV_MAT_ELEM(*distortion, float, 2, 0) = config.tangentialCoef.at(0);
  CV_MAT_ELEM(*distortion, float, 3, 0) = config.tangentialCoef.at(1);
  CV_MAT_ELEM(*distortion, float, 4, 0) = config.radialCoef.at(2);

  cvInitUndistortMap(intrinsic, distortion, this->mapx, this->mapy);

  cvRelease((void**)&intrinsic);
  cvRelease((void**)&distortion);
}

HardCamera::~HardCamera()
{
  cvReleaseImage(&(this->mapx));
  cvReleaseImage(&(this->mapy));
}

unsigned
HardCamera::addClient()
{
  this->unreadImage.push_back(false);
  return (this->unreadImage.size() - 1);
}

void
HardCamera::createIplImage(IplImage** iplImage) const
{
  *iplImage = cvCreateImage(cvSize(this->frameWidth, this->frameHeight),
                            IPL_DEPTH_8U,
                            3);
}

void
HardCamera::undistortFrame(IplImage* frame)
{
  IplImage *tmp = cvCloneImage(frame);

  cvRemap(tmp, frame, this->mapx, this->mapy,
          CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(0));

  cvReleaseImage(&tmp);
}

void
HardCamera::captureRectFrame(IplImage** image, unsigned clientUid)
{
  this->captureFrame(image, clientUid);
  undistortFrame(*image);
}
