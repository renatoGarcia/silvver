/*
 * 1394-Based Digital Camera Control Library
 *
 * Color conversion functions, including Bayer pattern decoding
 *
 * Written by Damien Douxchamps and Frederic Devernay
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __DC1394_CONVERSIONS_H__
#define __DC1394_CONVERSIONS_H__

#include <stdint.h>

/*! \file dc1394/conversions.h
    \brief functions to convert video formats
    \author Damien Douxchamps: coding
    \author Frederic Devernay: coding
    \author Peter Antoniac: documentation maintainer

    More details soon
*/

typedef enum {
    LIBDC1394_SUCCESS                     =  0,
    LIBDC1394_FAILURE                     = -1,
    LIBDC1394_NOT_A_CAMERA                = -2,
    LIBDC1394_FUNCTION_NOT_SUPPORTED      = -3,
    LIBDC1394_CAMERA_NOT_INITIALIZED      = -4,
    LIBDC1394_MEMORY_ALLOCATION_FAILURE   = -5,
    LIBDC1394_TAGGED_REGISTER_NOT_FOUND   = -6,
    LIBDC1394_NO_ISO_CHANNEL              = -7,
    LIBDC1394_NO_BANDWIDTH                = -8,
    LIBDC1394_IOCTL_FAILURE               = -9,
    LIBDC1394_CAPTURE_IS_NOT_SET          = -10,
    LIBDC1394_CAPTURE_IS_RUNNING          = -11,
    LIBDC1394_RAW1394_FAILURE             = -12,
    LIBDC1394_FORMAT7_ERROR_FLAG_1        = -13,
    LIBDC1394_FORMAT7_ERROR_FLAG_2        = -14,
    LIBDC1394_INVALID_ARGUMENT_VALUE      = -15,
    LIBDC1394_REQ_VALUE_OUTSIDE_RANGE     = -16,
    LIBDC1394_INVALID_FEATURE             = -17,
    LIBDC1394_INVALID_VIDEO_FORMAT        = -18,
    LIBDC1394_INVALID_VIDEO_MODE          = -19,
    LIBDC1394_INVALID_FRAMERATE           = -20,
    LIBDC1394_INVALID_TRIGGER_MODE        = -21,
    LIBDC1394_INVALID_TRIGGER_SOURCE      = -22,
    LIBDC1394_INVALID_ISO_SPEED           = -23,
    LIBDC1394_INVALID_IIDC_VERSION        = -24,
    LIBDC1394_INVALID_COLOR_CODING        = -25,
    LIBDC1394_INVALID_COLOR_FILTER        = -26,
    LIBDC1394_INVALID_CAPTURE_POLICY      = -27,
    LIBDC1394_INVALID_ERROR_CODE          = -28,
    LIBDC1394_INVALID_BAYER_METHOD        = -29,
    LIBDC1394_INVALID_VIDEO1394_DEVICE    = -30,
    LIBDC1394_INVALID_OPERATION_MODE      = -31,
    LIBDC1394_INVALID_TRIGGER_POLARITY    = -32,
    LIBDC1394_INVALID_FEATURE_MODE        = -33,
    LIBDC1394_INVALID_LOG_TYPE            = -34,
    LIBDC1394_INVALID_BYTE_ORDER          = -35,
    LIBDC1394_INVALID_STEREO_METHOD       = -36,
    LIBDC1394_BASLER_NO_MORE_SFF_CHUNKS   = -37,
    LIBDC1394_BASLER_CORRUPTED_SFF_CHUNK  = -38,
    LIBDC1394_BASLER_UNKNOWN_SFF_CHUNK    = -39
} libdc1394error_t;

/**
 * Enumeration of colour codings. For details on the data format please read the IIDC specifications.
 */
typedef enum {
    LIBDC1394_COLOR_CODING_MONO8= 352,
    LIBDC1394_COLOR_CODING_YUV411,
    LIBDC1394_COLOR_CODING_YUV422,
    LIBDC1394_COLOR_CODING_YUV444,
    LIBDC1394_COLOR_CODING_RGB8,
    LIBDC1394_COLOR_CODING_MONO16,
    LIBDC1394_COLOR_CODING_RGB16,
    LIBDC1394_COLOR_CODING_MONO16S,
    LIBDC1394_COLOR_CODING_RGB16S,
    LIBDC1394_COLOR_CODING_RAW8,
    LIBDC1394_COLOR_CODING_RAW16
} libdc1394color_coding_t;
#define LIBDC1394_COLOR_CODING_MIN     LIBDC1394_COLOR_CODING_MONO8
#define LIBDC1394_COLOR_CODING_MAX     LIBDC1394_COLOR_CODING_RAW16
#define LIBDC1394_COLOR_CODING_NUM    (LIBDC1394_COLOR_CODING_MAX - LIBDC1394_COLOR_CODING_MIN + 1)

/**
 * RAW sensor filters. These elementary tiles tesselate the image plane in RAW modes. RGGB should be interpreted in 2D as
 *
 *    RG
 *    GB
 *
 * and similarly for other filters.
 */
typedef enum {
    LIBDC1394_COLOR_FILTER_RGGB = 512,
    LIBDC1394_COLOR_FILTER_GBRG,
    LIBDC1394_COLOR_FILTER_GRBG,
    LIBDC1394_COLOR_FILTER_BGGR
} libdc1394color_filter_t;
#define LIBDC1394_COLOR_FILTER_MIN        LIBDC1394_COLOR_FILTER_RGGB
#define LIBDC1394_COLOR_FILTER_MAX        LIBDC1394_COLOR_FILTER_BGGR
#define LIBDC1394_COLOR_FILTER_NUM       (LIBDC1394_COLOR_FILTER_MAX - LIBDC1394_COLOR_FILTER_MIN + 1)

/**
 * Yet another boolean data type
 */
typedef enum {
    LIBDC1394_FALSE= 0,
    LIBDC1394_TRUE
} libdc1394bool_t;

/**
 * Byte order for YUV formats (may be expanded to RGB in the future)
 *
 * IIDC cameras always return data in UYVY order, but conversion functions can change this if requested.
 */
typedef enum {
    LIBDC1394_BYTE_ORDER_UYVY=800,
    LIBDC1394_BYTE_ORDER_YUYV
} libdc1394byte_order_t;
#define LIBDC1394_BYTE_ORDER_MIN        LIBDC1394_BYTE_ORDER_UYVY
#define LIBDC1394_BYTE_ORDER_MAX        LIBDC1394_BYTE_ORDER_YUYV
#define LIBDC1394_BYTE_ORDER_NUM       (LIBDC1394_BYTE_ORDER_MAX - LIBDC1394_BYTE_ORDER_MIN + 1)


#define restrict __restrict

/**
 * A list of de-mosaicing techniques for Bayer-patterns.
 *
 * The speed of the techniques can vary greatly, as well as their quality.
 */
typedef enum {
    LIBDC1394_BAYER_METHOD_NEAREST=0,
    LIBDC1394_BAYER_METHOD_SIMPLE,
    LIBDC1394_BAYER_METHOD_BILINEAR,
    LIBDC1394_BAYER_METHOD_HQLINEAR,
    LIBDC1394_BAYER_METHOD_DOWNSAMPLE,
    LIBDC1394_BAYER_METHOD_EDGESENSE,
    LIBDC1394_BAYER_METHOD_VNG,
    LIBDC1394_BAYER_METHOD_AHD
} dc1394bayer_method_t;
#define LIBDC1394_BAYER_METHOD_MIN      LIBDC1394_BAYER_METHOD_NEAREST
#define LIBDC1394_BAYER_METHOD_MAX      LIBDC1394_BAYER_METHOD_AHD
#define LIBDC1394_BAYER_METHOD_NUM     (LIBDC1394_BAYER_METHOD_MAX-LIBDC1394_BAYER_METHOD_MIN+1)

/**
 * A list of known stereo-in-normal-video modes used by manufacturers like Point Grey Research and Videre Design.
 */
typedef enum {
    LIBDC1394_STEREO_METHOD_INTERLACED=0,
    LIBDC1394_STEREO_METHOD_FIELD
} dc1394stereo_method_t;
#define LIBDC1394_STEREO_METHOD_MIN     LIBDC1394_STEREO_METHOD_INTERLACE
#define LIBDC1394_STEREO_METHOD_MAX     LIBDC1394_STEREO_METHOD_FIELD
#define LIBDC1394_STEREO_METHOD_NUM    (LIBDC1394_STEREO_METHOD_MAX-LIBDC1394_STEREO_METHOD_MIN+1)


// color conversion functions from Bart Nabbe.
// corrected by Damien: bad coeficients in YUV2RGB
#define YUV2RGB(y, u, v, r, g, b) {\
  r = y + ((v*1436) >> 10);\
  g = y - ((u*352 + v*731) >> 10);\
  b = y + ((u*1814) >> 10);\
  r = r < 0 ? 0 : r;\
  g = g < 0 ? 0 : g;\
  b = b < 0 ? 0 : b;\
  r = r > 255 ? 255 : r;\
  g = g > 255 ? 255 : g;\
  b = b > 255 ? 255 : b; }


#define RGB2YUV(r, g, b, y, u, v) {\
  y = (306*r + 601*g + 117*b)  >> 10;\
  u = ((-172*r - 340*g + 512*b) >> 10)  + 128;\
  v = ((512*r - 429*g - 83*b) >> 10) + 128;\
  y = y < 0 ? 0 : y;\
  u = u < 0 ? 0 : u;\
  v = v < 0 ? 0 : v;\
  y = y > 255 ? 255 : y;\
  u = u > 255 ? 255 : u;\
  v = v > 255 ? 255 : v; }

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************
 *  CONVERSION FUNCTIONS TO YUV422, MONO8 and RGB8
 **********************************************************************/

/**
 * Converts an image buffer to YUV422
 */
libdc1394error_t
dc1394_convert_to_YUV422(uint8_t *src, uint8_t *dest, uint32_t width, uint32_t height, uint32_t byte_order,
                         libdc1394color_coding_t source_coding, uint32_t bits);

/**
 * Converts an image buffer to MONO8
 */
libdc1394error_t
dc1394_convert_to_MONO8(uint8_t *src, uint8_t *dest, uint32_t width, uint32_t height, uint32_t byte_order,
                        libdc1394color_coding_t source_coding, uint32_t bits);

/**
 * Converts an image buffer to RGB8
 */
libdc1394error_t
dc1394_convert_to_RGB8(uint8_t *src, uint8_t *dest, uint32_t width, uint32_t height, uint32_t byte_order,
                       libdc1394color_coding_t source_coding, uint32_t bits);

/**********************************************************************
 *  CONVERSION FUNCTIONS FOR STEREO IMAGES
 **********************************************************************/

/**
 * changes a 16bit stereo image (8bit/channel) into two 8bit images on top of each other
 */
libdc1394error_t
dc1394_deinterlace_stereo(uint8_t *src, uint8_t *dest, uint32_t width, uint32_t height);

/************************************************************************************************
 *                                                                                              *
 *      Color conversion functions for cameras that can output raw Bayer pattern images (color  *
 *  codings LIBDC1394_COLOR_CODING_RAW8 and LIBDC1394_COLOR_CODING_RAW16).                            *
 *                                                                                              *
 *  Credits and sources:                                                                        *
 *  - Nearest Neighbor : OpenCV library                                                         *
 *  - Bilinear         : OpenCV library                                                         *
 *  - HQLinear         : High-Quality Linear Interpolation For Demosaicing Of Bayer-Patterned   *
 *                       Color Images, by Henrique S. Malvar, Li-wei He, and Ross Cutler,       *
 *                          in Proceedings of the ICASSP'04 Conference.                            *
 *  - Edge Sense II    : Laroche, Claude A. "Apparatus and method for adaptively interpolating  *
 *                       a full color image utilizing chrominance gradients"                    *
 *                          U.S. Patent 5,373,322. Based on the code found on the website          *
 *                       http://www-ise.stanford.edu/~tingchen/ Converted to C and adapted to   *
 *                       all four elementary patterns.                                          *
 *  - Downsample       : "Known to the Ancients"                                                *
 *  - Simple           : Implemented from the information found in the manual of Allied Vision  *
 *                       Technologies (AVT) cameras.                                            *
 *  - VNG              : Variable Number of Gradients, a method described in                    *
 *                       http://www-ise.stanford.edu/~tingchen/algodep/vargra.html              *
 *                       Sources import from DCRAW by Frederic Devernay. DCRAW is a RAW         *
 *                       converter program by Dave Coffin. URL:                                 *
 *                       http://www.cybercom.net/~dcoffin/dcraw/                                *
 *  - AHD              : Adaptive Homogeneity-Directed Demosaicing Algorithm, by K. Hirakawa    *
 *                       and T.W. Parks, IEEE Transactions on Image Processing, Vol. 14, Nr. 3, *
 *                       March 2005, pp. 360 - 369.                                             *
 *                                                                                              *
 ************************************************************************************************/

/**
 * Perform de-mosaicing on an 8-bit image buffer
 */
libdc1394error_t
dc1394_bayer_decoding_8bit(const uint8_t *bayer, uint8_t *rgb,
                           uint32_t width, uint32_t height, libdc1394color_filter_t tile,
                           dc1394bayer_method_t method);

/**
 * Perform de-mosaicing on an 16-bit image buffer
 */
libdc1394error_t
dc1394_bayer_decoding_16bit(const uint16_t *bayer, uint16_t *rgb,
                            uint32_t width, uint32_t height, libdc1394color_filter_t tile,
                            dc1394bayer_method_t method, uint32_t bits);

#ifdef __cplusplus
}
#endif

#endif /* _DC1394_CONVERSIONS_H */
