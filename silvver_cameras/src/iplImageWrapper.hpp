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

#include <opencv/cv.h>

#include <cstddef>

class IplImageWrapper
{
public:

  /// Default constructor.
  /// Build a empty image, don't allocating any data.
  IplImageWrapper();

  IplImageWrapper(CvSize size, int depth, int channels);

  /// Construct a new IplImageWrapper cloning the iplImageSrc.
  /// @param iplImageSrc Pointer to image which will be cloned.
  IplImageWrapper(IplImage* iplImageSrc);

  /// IplImageWrapper copy constructor.
  /// @param iplImageWrapper Source image.
  IplImageWrapper(const IplImageWrapper& iplImageWrapper);

  ~IplImageWrapper();

  const IplImageWrapper& operator=(const IplImageWrapper& iplImageWrapper);

  operator const IplImage* const() const;

  operator IplImage*();

  /// Get the size of image.
  /// @return A CvSize representing the image size.
  CvSize size() const;

  int depth() const;

  int channels() const;

  const char* const data() const;

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
IplImageWrapper::IplImageWrapper(CvSize size, int depth, int channels)
  :iplImage(cvCreateImage(size, depth, channels))
{}

inline
IplImageWrapper::IplImageWrapper(IplImage* iplImageSrc)
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
IplImageWrapper::operator const IplImage* const() const
{
  return this->iplImage;
}

inline
IplImageWrapper::operator IplImage*()
{
  return this->iplImage;
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

inline const char* const
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
