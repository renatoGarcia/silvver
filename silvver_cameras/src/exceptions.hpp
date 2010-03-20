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

#ifndef _EXCEPTIONS_HPP_
#define _EXCEPTIONS_HPP_

#include <boost/array.hpp>
#include <boost/exception/all.hpp>
#include <boost/tuple/tuple.hpp>
#include <exception>
#include <string>

struct silvver_cameras_exception: public virtual std::exception,
                                  public virtual boost::exception
{
  friend std::string errorsInfo2string(const silvver_cameras_exception& e);
  const char* what() const throw();
};

struct invalid_argument: public virtual silvver_cameras_exception {};
struct load_file_error: public virtual silvver_cameras_exception {};
struct open_camera_error: public virtual silvver_cameras_exception {};
struct camera_parameter_error: public virtual silvver_cameras_exception {};

struct init_target_error: public virtual silvver_cameras_exception {};

struct file_parsing_error: public virtual silvver_cameras_exception {};
struct missing_field: public virtual file_parsing_error {};
struct type_error: public virtual file_parsing_error {};

std::string errorsInfo2string(const silvver_cameras_exception& e);

typedef boost::error_info<struct tag_what, std::string>
  info_what;
//-------------
typedef boost::error_info<struct tag_cameraModel, std::string>
  info_cameraModel;

typedef boost::error_info<struct tag_cameraUid, std::string>
  info_cameraUid;

typedef boost::error_info<struct tag_colorMode, std::string>
  info_colorMode;

typedef boost::error_info<struct tag_resolution, boost::array<unsigned, 2> >
  info_resolution;

typedef boost::error_info<struct tag_frameRate, float>
  info_frameRate;

typedef boost::error_info<struct tag_featureName, std::string>
  info_featureName;

typedef boost::error_info<struct tag_featureValue, std::string>
  info_featureValue;

typedef boost::error_info<struct tag_featureRange, std::string>
  info_featureRange;

typedef boost::error_info<struct tag_bayer, std::string>
  info_bayer;

typedef boost::error_info<struct tag_colorFilter, std::string>
  info_colorFilter;
//-------------
typedef boost::error_info<struct tag_cameraIndex, int>
  info_cameraIndex;

typedef boost::error_info<struct tag_targetIndex, int>
  info_targetIndex;

typedef boost::error_info<struct tag_fieldName, std::string>
  info_fieldName;

typedef boost::error_info<struct tag_arrayIndex, int>
  info_arrayIndex;



#endif /* _EXCEPTIONS_HPP_ */
