/* Copyright 2009 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#ifndef _IPLIMAGEWRAPPER_HPP_
#define _IPLIMAGEWRAPPER_HPP_

#include <boost/tuple/tuple.hpp>
#include <cstddef>
#include <string>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "exceptions.hpp"

class IplImageWrapper
{
public:

  /// Tuple with image size, depth and number of channels.
  typedef boost::tuple<CvSize,int,int> IplParameters;

  /// Default constructor.
  /// Build a empty image, allocating no data.
  IplImageWrapper();

  IplImageWrapper(const IplParameters& parameters);

  IplImageWrapper(const CvSize& size, int depth, int nChannels);

  /// Construct a new IplImageWrapper cloning the iplImageSrc.
  /// @param iplImageSrc Pointer to image which will be cloned.
  IplImageWrapper(const IplImage* const iplImageSrc);

  /// IplImageWrapper copy constructor.
  /// @param iplImageWrapper Source image.
  IplImageWrapper(const IplImageWrapper& iplImageWrapper);

  ~IplImageWrapper();

  const IplImageWrapper& operator=(const IplImageWrapper& iplImageWrapper);

  operator const IplImage*() const;

  operator IplImage*();

  /// Load an image in disk.
  /// This method can throw load_file_error.
  /// @param filename The path to image.
  /// @param iscolor Same as cvLoadImage,
  ///                CV_LOAD_IMAGE_<COLOR|GRAYSCALE|UNCHANGED>.
  void loadImage(const std::string& filename,
                 int iscolor=CV_LOAD_IMAGE_UNCHANGED);

  /// Transform the color space of IplImageWrapper.
  /// @param code The same code used in cvCvt function.
  void convertColor(int code);

  /// Transform the color space of IplImageWrapper.
  /// @param destine The IplImageWrapper where apply the result.
  /// @param code The same code used in cvCvt function.
  void convertColor(IplImageWrapper& destine, int code) const;

  /// Get the size of image.
  /// @return A CvSize representing the image size.
  CvSize size() const;

  int depth() const;

  int channels() const;

  const char* data() const;

  /// Get the image data.
  /// @return A char* to beginning of image data.
  char* data();

private:

  IplImage* iplImage;
};



inline
IplImageWrapper::IplImageWrapper()
  :iplImage(NULL)
{}

inline
IplImageWrapper::IplImageWrapper(const IplParameters& parameters)
  :iplImage(cvCreateImage(parameters.get<0>(), parameters.get<1>(),
                          parameters.get<2>()))
{}

inline
IplImageWrapper::IplImageWrapper(const CvSize& size, int depth, int nChannels)
  :iplImage(cvCreateImage(size, depth, nChannels))
{}

inline
IplImageWrapper::IplImageWrapper(const IplImage* const iplImageSrc)
  :iplImage(cvCloneImage(iplImageSrc))
{}

inline
IplImageWrapper::IplImageWrapper(const IplImageWrapper& iplImageWrapper)
  :iplImage(cvCloneImage(iplImageWrapper.iplImage))
{}

inline
IplImageWrapper::~IplImageWrapper()
{
  if (this->iplImage)
  {
    cvReleaseImage(&this->iplImage);
  }
}

inline const IplImageWrapper&
IplImageWrapper::operator=(const IplImageWrapper& iplImageWrapper)
{
  // If is a self-assignment
  if (this == &iplImageWrapper)
  {
    return *this;
  }

  if (this->iplImage)
  {
    cvReleaseImage(&this->iplImage);
  }
  this->iplImage = cvCloneImage(iplImageWrapper.iplImage);

  return *this;
}

inline
IplImageWrapper::operator const IplImage*() const
{
  return this->iplImage;
}

inline
IplImageWrapper::operator IplImage*()
{
  return this->iplImage;
}

inline void
IplImageWrapper::loadImage(const std::string& filename, int iscolor)
{
  if (this->iplImage)
  {
    cvReleaseImage(&this->iplImage);
  }

 this->iplImage = cvLoadImage(filename.c_str(), iscolor);

  if (this->iplImage == NULL)
  {
    throw load_file_error("Unable to load image ")
      << boost::errinfo_file_name(filename);
  }
}

inline void
IplImageWrapper::convertColor(int code)
{
  IplImageWrapper tmpImage(*this);
  cvCvtColor(tmpImage.iplImage, this->iplImage, code);
}

inline void
IplImageWrapper::convertColor(IplImageWrapper& destine, int code) const
{
  cvCvtColor(this->iplImage, destine.iplImage, code);
}

inline CvSize
IplImageWrapper::size() const
{
  return this->iplImage ?
    cvSize(this->iplImage->width, this->iplImage->height) : cvSize(0,0);
}

inline int
IplImageWrapper::depth() const
{
  return this->iplImage ? this->iplImage->depth : 0;
}

inline int
IplImageWrapper::channels() const
{
  return this->iplImage ? this->iplImage->nChannels : 0;
}

inline const char*
IplImageWrapper::data() const
{
  return this->iplImage ? this->iplImage->imageData : 0;
}

inline char*
IplImageWrapper::data()
{
  return this->iplImage ? this->iplImage->imageData : 0;
}

#endif /* _IPLIMAGEWRAPPER_HPP_ */
