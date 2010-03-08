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

#include "colorConverter.hpp"

#include <stdexcept>

ColorConverter::ColorConverter(const ColorSpace& colorSpace,
                               const unsigned width, const unsigned height)
  :colorSpace(colorSpace)
  ,imgWidth(width)
  ,imgHeight(height)
{
  switch (colorSpace.value())
  {
  case ColorSpace::RGB8:
    this->convertFunc = &ColorConverter::rgb;
    break;
  case ColorSpace::YUV411:
    this->convertFunc = &ColorConverter::yuv411;
    break;
  case ColorSpace::YUYV:
    this->convertFunc = &ColorConverter::yuyv;
    break;
  case ColorSpace::UYVY:
    this->convertFunc = &ColorConverter::uyvy;
    break;
  case ColorSpace::MONO8:
    this->convertFunc = &ColorConverter::mono8;
    break;
  case ColorSpace::MONO16:
    this->convertFunc = &ColorConverter::mono16;
    break;
  default:
    throw std::invalid_argument("When using color space RAW8 or RAW16, the "
                                "color filter and the bayer method must be "
                                "informed.");

  }
}

ColorConverter::ColorConverter(const ColorSpace& colorSpace,
                               const unsigned width, const unsigned height,
                               const ColorFilter& filter,
                               const BayerMethod& bayer)
  :colorSpace(colorSpace)
  ,imgWidth(width)
  ,imgHeight(height)
  ,colorFilter(filter)
{
  if (colorSpace == ColorSpace::RAW8)
  {
    switch (bayer.value())
    {
    case BayerMethod::NEAREST:
      this->convertFunc = &ColorConverter::bayer_NearestNeighbor<uint8_t>;
      break;
    case BayerMethod::BILINEAR:
      this->convertFunc = &ColorConverter::bayer_Bilinear<uint8_t>;
      break;
    }
  }
  else if (colorSpace == ColorSpace::RAW16)
  {
    switch (bayer.value())
    {
    case BayerMethod::NEAREST:
      this->convertFunc = &ColorConverter::bayer_NearestNeighbor<uint16_t>;
      break;
    case BayerMethod::BILINEAR:
      this->convertFunc = &ColorConverter::bayer_Bilinear<uint16_t>;
      break;
    }
  }
  else
  {
    throw std::invalid_argument("The color space to bayer operations must be "
                                "either RAW8 or RAW16, tried use " +
                                std::string(bayer.str()));
  }
}

ColorConverter::~ColorConverter()
{}

// color conversion functions from Bart Nabbe.
// corrected by Damien: bad coeficients in YUV2RGB
inline void
ColorConverter::yuv2rgb(const int y, const int u, const int v,
                        int& r, int& g, int& b) const
{
  r = y + ((v*1436) >> 10);
  g = y - ((u*352 + v*731) >> 10);
  b = y + ((u*1814) >> 10);
  r = r < 0 ? 0 : r;
  g = g < 0 ? 0 : g;
  b = b < 0 ? 0 : b;
  r = r > 255 ? 255 : r;
  g = g > 255 ? 255 : g;
  b = b > 255 ? 255 : b;
}

template <class Type>
inline void
ColorConverter::ClearBorders(Type* rgb, const int w) const
{
  int i, j;
  // black edges are added with a width w:
  i = 3 * this->imgWidth * w - 1;
  j = 3 * this->imgWidth * this->imgHeight - 1;
  while (i >= 0) {
    rgb[i--] = 0;
    rgb[j--] = 0;
  }

  int low = this->imgWidth * (w - 1) * 3 - 1 + w * 3;
  i = low + this->imgWidth * (this->imgHeight - w * 2 + 1) * 3;
  while (i > low) {
    j = 6 * w;
    while (j > 0) {
      rgb[i--] = 0;
      j--;
    }
    i -= (this->imgWidth - 2 * w) * 3;
  }
}

void
ColorConverter::yuv411(const uint8_t* restrict yuv,
                       uint8_t* restrict rgb) const
{
  register int i = (this->imgWidth * this->imgHeight) +
                   ((this->imgWidth * this->imgHeight) >> 1) - 1;
  register int j = (this->imgWidth * this->imgHeight) +
                   ((this->imgWidth * this->imgHeight) << 1) - 1;
  register int y0, y1, y2, y3, u, v;
  register int r, g, b;

  while (i >= 0) {
    y3 = (uint8_t) yuv[i--];
    y2 = (uint8_t) yuv[i--];
    v  = (uint8_t) yuv[i--] - 128;
    y1 = (uint8_t) yuv[i--];
    y0 = (uint8_t) yuv[i--];
    u  = (uint8_t) yuv[i--] - 128;
    yuv2rgb(y3, u, v, r, g, b);
    rgb[j--] = b;
    rgb[j--] = g;
    rgb[j--] = r;
    yuv2rgb(y2, u, v, r, g, b);
    rgb[j--] = b;
    rgb[j--] = g;
    rgb[j--] = r;
    yuv2rgb(y1, u, v, r, g, b);
    rgb[j--] = b;
    rgb[j--] = g;
    rgb[j--] = r;
    yuv2rgb(y0, u, v, r, g, b);
    rgb[j--] = b;
    rgb[j--] = g;
    rgb[j--] = r;
  }
}

void
ColorConverter::yuyv(const uint8_t* restrict yuv, uint8_t* restrict rgb) const
{
  register int i = ((this->imgWidth * this->imgHeight) << 1) - 1;
  register int j = (this->imgWidth * this->imgHeight) +
                   ((this->imgWidth * this->imgHeight) << 1) - 1;
  register int y0, y1, u, v;
  register int r, g, b;

  while (i >= 0)
  {
    v  = (uint8_t) yuv[i--] -128;
    y1 = (uint8_t) yuv[i--];
    u  = (uint8_t) yuv[i--] -128;
    y0 = (uint8_t) yuv[i--];
    yuv2rgb(y1, u, v, r, g, b);
    rgb[j--] = b;
    rgb[j--] = g;
    rgb[j--] = r;
    yuv2rgb(y0, u, v, r, g, b);
    rgb[j--] = b;
    rgb[j--] = g;
    rgb[j--] = r;
  }
}

void
ColorConverter::uyvy(const uint8_t* restrict yuv, uint8_t* restrict rgb) const
{
  register int i = ((this->imgWidth * this->imgHeight) << 1) - 1;
  register int j = (this->imgWidth * this->imgHeight) +
                   ((this->imgWidth * this->imgHeight) << 1) - 1;
  register int y0, y1, u, v;
  register int r, g, b;

  while (i >= 0)
  {
    y1 = (uint8_t) yuv[i--];
    v  = (uint8_t) yuv[i--] - 128;
    y0 = (uint8_t) yuv[i--];
    u  = (uint8_t) yuv[i--] - 128;
    yuv2rgb(y1, u, v, r, g, b);
    rgb[j--] = b;
    rgb[j--] = g;
    rgb[j--] = r;
    yuv2rgb(y0, u, v, r, g, b);
    rgb[j--] = b;
    rgb[j--] = g;
    rgb[j--] = r;
  }
}

void
ColorConverter::rgb(const uint8_t* restrict rgb, uint8_t* restrict bgr) const
{
}

void
ColorConverter::mono8(const uint8_t* restrict mono,
                      uint8_t* restrict rgb) const
{
  register int i = (this->imgWidth * this->imgHeight) - 1;
  register int j = (this->imgWidth * this->imgHeight) +
                   ((this->imgWidth * this->imgHeight) << 1) - 1;
  register int y;

  while (i >= 0)
  {
    y = (uint8_t) mono[i--];
    rgb[j--] = y;
    rgb[j--] = y;
    rgb[j--] = y;
  }
}

void
ColorConverter::mono16(const uint8_t* restrict mono,
                       uint8_t* restrict rgb) const
{
  register int i = ((this->imgWidth * this->imgHeight) << 1) - 1;
  register int j = (this->imgWidth * this->imgHeight) +
                   ((this->imgWidth * this->imgHeight) << 1) - 1;
  register int y;

  while (i > 0)
  {
    y = mono[i--];
    y = (y + (mono[i--] << 8)) >> 8;
    rgb[j--] = y;
    rgb[j--] = y;
    rgb[j--] = y;
  }
}

template <class Type>
void
ColorConverter::bayer_NearestNeighbor(const uint8_t* restrict inputBayer,
                                      uint8_t* restrict outputRgb) const
{
  Type* restrict bayer = (Type*) inputBayer;
  Type* restrict rgb = (Type*) outputRgb;

  const int bayerStep = this->imgWidth;
  const int rgbStep = 3 * this->imgWidth;
  int width = this->imgWidth;
  int height = this->imgHeight;
  int blue = this->colorFilter == ColorFilter::BGGR ||
             this->colorFilter == ColorFilter::GBRG ? -1 : 1;
  int start_with_green = this->colorFilter == ColorFilter::GBRG ||
                         this->colorFilter == ColorFilter::GRBG;
  int i, imax, iinc;

  /* add black border */
  imax = this->imgWidth * this->imgHeight * 3;
  for (i = this->imgWidth * (this->imgHeight - 1) * 3; i < imax; i++)
  {
    rgb[i] = 0;
  }
  iinc = (this->imgWidth - 1) * 3;
  for (i = (this->imgWidth - 1) * 3; i < imax; i += iinc) {
    rgb[i++] = 0;
    rgb[i++] = 0;
    rgb[i++] = 0;
  }

  rgb += 1;
  width -= 1;
  height -= 1;

  for (; height--; bayer += bayerStep, rgb += rgbStep) {
    //int t0, t1;
    const Type* bayerEnd = bayer + width;

    if (start_with_green) {
      rgb[-blue] = bayer[1];
      rgb[0] = bayer[bayerStep + 1];
      rgb[blue] = bayer[bayerStep];
      bayer++;
      rgb += 3;
    }

    if (blue > 0) {
      for (; bayer <= bayerEnd - 2; bayer += 2, rgb += 6) {
        rgb[-1] = bayer[0];
        rgb[0] = bayer[1];
        rgb[1] = bayer[bayerStep + 1];

        rgb[2] = bayer[2];
        rgb[3] = bayer[bayerStep + 2];
        rgb[4] = bayer[bayerStep + 1];
      }
    } else {
      for (; bayer <= bayerEnd - 2; bayer += 2, rgb += 6) {
        rgb[1] = bayer[0];
        rgb[0] = bayer[1];
        rgb[-1] = bayer[bayerStep + 1];

        rgb[4] = bayer[2];
        rgb[3] = bayer[bayerStep + 2];
        rgb[2] = bayer[bayerStep + 1];
      }
    }

    if (bayer < bayerEnd) {
      rgb[-blue] = bayer[0];
      rgb[0] = bayer[1];
      rgb[blue] = bayer[bayerStep + 1];
      bayer++;
      rgb += 3;
    }

    bayer -= width;
    rgb -= width * 3;

    blue = -blue;
    start_with_green = !start_with_green;
  }
}

/* OpenCV's Bayer decoding */
template <class Type>
void
ColorConverter::bayer_Bilinear(const uint8_t* restrict inputBayer,
                               uint8_t* restrict outputRgb) const
{
  Type* restrict bayer = (Type*) inputBayer;
  Type* restrict rgb = (Type*) outputRgb;

  const int bayerStep = this->imgWidth;
  const int rgbStep = 3 * this->imgWidth;
  int width = this->imgWidth;
  int height = this->imgHeight;
  /*
    the two letters  of the OpenCV name are respectively
    the 4th and 3rd letters from the blinky name,
    and we also have to switch R and B (OpenCV is BGR)

    CV_BayerBG2BGR <-> LIBDC1394_COLOR_FILTER_BGGR
    CV_BayerGB2BGR <-> LIBDC1394_COLOR_FILTER_GBRG
    CV_BayerGR2BGR <-> LIBDC1394_COLOR_FILTER_GRBG

    int blue = tile == CV_BayerBG2BGR || tile == CV_BayerGB2BGR ? -1 : 1;
    int start_with_green = tile == CV_BayerGB2BGR || tile == CV_BayerGR2BGR;
  */
  int blue = this->colorFilter == ColorFilter::BGGR ||
             this->colorFilter == ColorFilter::GBRG ? -1 : 1;
  int start_with_green = this->colorFilter == ColorFilter::GBRG ||
                         this->colorFilter == ColorFilter::GRBG;

  ClearBorders(rgb, 1);
  rgb += rgbStep + 3 + 1;
  height -= 2;
  width -= 2;

  for (; height--; bayer += bayerStep, rgb += rgbStep) {
    int t0, t1;
    const Type *bayerEnd = bayer + width;

    if (start_with_green) {
      /* OpenCV has a bug in the next line, which was
         t0 = (bayer[0] + bayer[bayerStep * 2] + 1) >> 1; */
      t0 = (bayer[1] + bayer[bayerStep * 2 + 1] + 1) >> 1;
      t1 = (bayer[bayerStep] + bayer[bayerStep + 2] + 1) >> 1;
      rgb[-blue] = (Type) t0;
      rgb[0] = bayer[bayerStep + 1];
      rgb[blue] = (Type) t1;
      bayer++;
      rgb += 3;
    }

    if (blue > 0) {
      for (; bayer <= bayerEnd - 2; bayer += 2, rgb += 6) {
        t0 = (bayer[0] + bayer[2] + bayer[bayerStep * 2] +
              bayer[bayerStep * 2 + 2] + 2) >> 2;
        t1 = (bayer[1] + bayer[bayerStep] +
              bayer[bayerStep + 2] + bayer[bayerStep * 2 + 1] +
              2) >> 2;
        rgb[-1] = (Type) t0;
        rgb[0] = (Type) t1;
        rgb[1] = bayer[bayerStep + 1];

        t0 = (bayer[2] + bayer[bayerStep * 2 + 2] + 1) >> 1;
        t1 = (bayer[bayerStep + 1] + bayer[bayerStep + 3] +
              1) >> 1;
        rgb[2] = (Type) t0;
        rgb[3] = bayer[bayerStep + 2];
        rgb[4] = (Type) t1;
      }
    } else {
      for (; bayer <= bayerEnd - 2; bayer += 2, rgb += 6) {
        t0 = (bayer[0] + bayer[2] + bayer[bayerStep * 2] +
              bayer[bayerStep * 2 + 2] + 2) >> 2;
        t1 = (bayer[1] + bayer[bayerStep] +
              bayer[bayerStep + 2] + bayer[bayerStep * 2 + 1] +
              2) >> 2;
        rgb[1] = (Type) t0;
        rgb[0] = (Type) t1;
        rgb[-1] = bayer[bayerStep + 1];

        t0 = (bayer[2] + bayer[bayerStep * 2 + 2] + 1) >> 1;
        t1 = (bayer[bayerStep + 1] + bayer[bayerStep + 3] +
              1) >> 1;
        rgb[4] = (Type) t0;
        rgb[3] = bayer[bayerStep + 2];
        rgb[2] = (Type) t1;
      }
    }

    if (bayer < bayerEnd) {
      t0 = (bayer[0] + bayer[2] + bayer[bayerStep * 2] +
            bayer[bayerStep * 2 + 2] + 2) >> 2;
      t1 = (bayer[1] + bayer[bayerStep] +
            bayer[bayerStep + 2] + bayer[bayerStep * 2 + 1] +
            2) >> 2;
      rgb[-blue] = (Type) t0;
      rgb[0] = (Type) t1;
      rgb[blue] = bayer[bayerStep + 1];
      bayer++;
      rgb += 3;
    }

    bayer -= width;
    rgb -= width * 3;

    blue = -blue;
    start_with_green = !start_with_green;
  }
}
