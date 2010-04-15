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

#ifndef _FRAME_HPP_
#define _FRAME_HPP_

#include <boost/tuple/tuple.hpp>
#include <cstddef>
#include <stdint.h>
#include <string>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "exceptions.hpp"

class Frame
{
public:

  /// Tuple with image size, depth and number of channels.
  typedef boost::tuple<CvSize,int,int> IplParameters;

  /// Default constructor.
  /// Build a empty image, allocating no data.
  Frame();

  Frame(const IplParameters& parameters);

  Frame(const CvSize& size, int depth, int nChannels);

  /// Construct a new Frame cloning the iplImageSrc.
  /// @param iplImageSrc Pointer to image which will be cloned.
  Frame(const IplImage* const iplImageSrc);

  /// Frame copy constructor.
  /// @param iplImageWrapper Source image.
  Frame(const Frame& iplImageWrapper);

  ~Frame();

  const Frame& operator=(const Frame& iplImageWrapper);

  operator const IplImage*() const;

  operator IplImage*();

  /// Load an image in disk.
  /// This method can throw load_file_error.
  /// @param filename The path to image.
  /// @param iscolor Same as cvLoadImage,
  ///                CV_LOAD_IMAGE_<COLOR|GRAYSCALE|UNCHANGED>.
  void loadImage(const std::string& filename,
                 int iscolor=CV_LOAD_IMAGE_UNCHANGED);

  /// Transform the color space of Frame.
  /// @param code The same code used in cvCvt function.
  void convertColor(int code);

  /// Transform the color space of Frame.
  /// @param destine The Frame where apply the result.
  /// @param code The same code used in cvCvt function.
  void convertColor(Frame& destine, int code) const;

  /// Get the size of image.
  /// @return A CvSize representing the image size.
  CvSize size() const;

  int depth() const;

  int channels() const;

  const char* data() const;

  /// Get the image data.
  /// @return A char* to beginning of image data.
  char* data();

  /// The instant of frame grab [microseconds].
  uint64_t timestamp;

private:
  IplImage* iplImage;
};



inline
Frame::Frame()
  :iplImage(NULL)
{}

inline
Frame::Frame(const IplParameters& parameters)
  :iplImage(cvCreateImage(parameters.get<0>(), parameters.get<1>(),
                          parameters.get<2>()))
{}

inline
Frame::Frame(const CvSize& size, int depth, int nChannels)
  :iplImage(cvCreateImage(size, depth, nChannels))
{}

inline
Frame::Frame(const IplImage* const iplImageSrc)
  :iplImage(cvCloneImage(iplImageSrc))
{}

inline
Frame::Frame(const Frame& iplImageWrapper)
  :iplImage(cvCloneImage(iplImageWrapper.iplImage))
{}

inline
Frame::~Frame()
{
  if (this->iplImage)
  {
    cvReleaseImage(&this->iplImage);
  }
}

inline const Frame&
Frame::operator=(const Frame& iplImageWrapper)
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
Frame::operator const IplImage*() const
{
  return this->iplImage;
}

inline
Frame::operator IplImage*()
{
  return this->iplImage;
}

inline void
Frame::loadImage(const std::string& filename, int iscolor)
{
  if (this->iplImage)
  {
    cvReleaseImage(&this->iplImage);
  }

 this->iplImage = cvLoadImage(filename.c_str(), iscolor);

  if (this->iplImage == NULL)
  {
    throw load_file_error()
      << info_what("Unable to load image ")
      << boost::errinfo_file_name(filename);
  }
}

inline void
Frame::convertColor(int code)
{
  Frame tmpImage(*this);
  cvCvtColor(tmpImage.iplImage, this->iplImage, code);
}

inline void
Frame::convertColor(Frame& destine, int code) const
{
  cvCvtColor(this->iplImage, destine.iplImage, code);
}

inline CvSize
Frame::size() const
{
  return this->iplImage ?
    cvSize(this->iplImage->width, this->iplImage->height) : cvSize(0,0);
}

inline int
Frame::depth() const
{
  return this->iplImage ? this->iplImage->depth : 0;
}

inline int
Frame::channels() const
{
  return this->iplImage ? this->iplImage->nChannels : 0;
}

inline const char*
Frame::data() const
{
  return this->iplImage ? this->iplImage->imageData : 0;
}

inline char*
Frame::data()
{
  return this->iplImage ? this->iplImage->imageData : 0;
}

#endif /* _FRAME_HPP_ */
