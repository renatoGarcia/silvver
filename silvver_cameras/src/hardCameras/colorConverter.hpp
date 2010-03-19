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

#ifndef _COLOR_CONVERTER_HPP_
#define _COLOR_CONVERTER_HPP_

#include <stdint.h>

#include "../iplImageWrapper.hpp"
#include "boost/enum.hpp"

#define restrict __restrict

BOOST_ENUM(ColorSpace,
           (NONE)
           (RGB8)
           (YUV411)
           (YUYV)
           (UYVY)
           (MONO8)
           (MONO16)
           (RAW8)
           (RAW16));

BOOST_ENUM(ColorFilter,
           (NONE)
           (RGGB)
           (GBRG)
           (GRBG)
           (BGGR));

BOOST_ENUM(BayerMethod,
           (NONE)
           (NEAREST)
           (BILINEAR));

class ColorConverter
{
public:

  ColorConverter(const ColorSpace& colorSpace,
                 const unsigned width, const unsigned height);

  ColorConverter(const ColorSpace& colorSpace,
                 const unsigned width, const unsigned height,
                 const ColorFilter& filter, const BayerMethod& bayer);

  ~ColorConverter();

  /// Converts the color space of input image to BGR.
  /// @param input Pointer to input image buffer.
  /// @param output IplImageWrapper where put the converted image.
  void operator()(const uint8_t* input, IplImageWrapper& output) const;

private:

  void (ColorConverter::*convertFunc)(const uint8_t*,uint8_t*) const;

  void yuv2rgb(const int y, const int u, const int v,
               int& r, int& g, int& b) const;

  template <class Type>
  void ClearBorders(Type *rgb, const int w) const;

  void yuv411(const uint8_t* restrict yuv, uint8_t* restrict rgb) const;

  /// YUV422 IN YUYV order
  void yuyv(const uint8_t* restrict yuv, uint8_t* restrict rgb) const;

  /// YUV422 IN UYVY order
  void uyvy(const uint8_t* restrict yuv, uint8_t* restrict rgb) const;

  void rgb(const uint8_t* restrict rgb, uint8_t* restrict bgr) const;

  void mono8(const uint8_t* restrict mono, uint8_t* restrict rgb) const;

  void mono16(const uint8_t* restrict mono, uint8_t* restrict rgb) const;

  template <class Type>
  void bayer_NearestNeighbor(const uint8_t* restrict inputBayer,
                             uint8_t* restrict outputRgb) const;

  template <class Type>
  void bayer_Bilinear(const uint8_t* restrict inputBayer,
                      uint8_t* restrict outputRgb) const;

  const ColorSpace colorSpace;

  const unsigned imgWidth;

  const unsigned imgHeight;

  const ColorFilter colorFilter;
};

inline void
ColorConverter::operator()(const uint8_t* input,
                           IplImageWrapper& output) const
{
  (this->*convertFunc)(input, (uint8_t*) output.data());
  output.convertColor(CV_RGB2BGR);
}

#endif /* _COLOR_CONVERTER_HPP_ */
