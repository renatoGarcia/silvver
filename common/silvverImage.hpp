/* Copyright 2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#ifndef _SILVVER_IMAGE_HPP_
#define _SILVVER_IMAGE_HPP_

#include <cstring>

#ifdef HAS_OPENCV
#  include <opencv/cv.h>
#endif

namespace silvver
{
  class Image
  {
  public:
    Image();

    Image(const Image& image);

#ifdef HAS_OPENCV
    Image(IplImage* iplImage);
#endif

    ~Image();

    const Image& operator=(const Image& image);

#ifdef HAS_OPENCV
    IplImage* toIplImage();
#endif

  // private:
    int  nChannels;
    int  depth;
    int  dataOrder;
    int  origin;
    int  width;
    int  height;
    int  imageSize;
    char* imageData;
    int  widthStep;
    char *imageDataOrigin;
  };

  inline
  Image::Image()
    :nChannels()
    ,depth()
    ,dataOrder()
    ,origin()
    ,width(0)
    ,height(0)
    ,imageSize(0)
    ,imageData(NULL)
    ,widthStep(0)
    ,imageDataOrigin(NULL)
  {}

  inline
  Image::Image(const Image& image)
    :nChannels(image.nChannels)
    ,depth(image.depth)
    ,dataOrder(image.dataOrder)
    ,origin(image.origin)
    ,width(image.width)
    ,height(image.height)
    ,imageSize(image.imageSize)
    ,imageData(NULL)
    ,widthStep(image.widthStep)
    ,imageDataOrigin(NULL)
  {
    this->imageData = new char[this->imageSize];
    memcpy(this->imageData, image.imageData, this->imageSize);
    this->imageDataOrigin = this->imageData;
  }

#ifdef HAS_OPENCV
  inline
  Image::Image(IplImage* iplImage)
    :nChannels(iplImage->nChannels)
    ,depth(iplImage->depth)
    ,dataOrder(iplImage->dataOrder)
    ,origin(iplImage->origin)
    ,width(iplImage->width)
    ,height(iplImage->height)
    ,imageSize(iplImage->imageSize)
    ,imageData(NULL)
    ,widthStep(iplImage->widthStep)
    ,imageDataOrigin(NULL)
  {
    this->imageData = new char[this->imageSize];
    memcpy(this->imageData, iplImage->imageData, this->imageSize);
    this->imageDataOrigin = this->imageData;
  }
#endif

  inline
  Image::~Image()
  {
    if (this->imageData)
    {
      delete[] this->imageData;
      this->imageDataOrigin = NULL;
    }
  }

  inline
  const Image&
  Image::operator=(const Image& image)
  {
    // If is a self-assignment
    if (this == &image)
    {
      return *this;
    }

    if (this->imageData)
    {
      delete[] this->imageData;
    }

    this->nChannels = image.nChannels;
    this->depth = image.depth;
    this->dataOrder = image.dataOrder;
    this->origin = image.origin;
    this->width = image.width;
    this->height = image.height;
    this->imageSize = image.imageSize;
    this->widthStep = image.widthStep;

    this->imageData = new char[this->imageSize];
    memcpy(this->imageData, image.imageData, this->imageSize);
    this->imageDataOrigin = this->imageData;

    return *this;
  }

} // Namespace silvver

#endif /* _SILVVER_IMAGE_HPP_ */
