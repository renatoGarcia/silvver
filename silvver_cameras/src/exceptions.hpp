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

  silvver_cameras_exception(const std::string& message="");
  const char* what() const throw();
  ~silvver_cameras_exception() throw();

private:
  std::string message;
};

struct invalid_argument: public virtual silvver_cameras_exception
{
  invalid_argument(const std::string& message="");
};

struct load_file_error: public virtual silvver_cameras_exception
{
  load_file_error(const std::string& message="");
};

struct open_camera_error: public virtual silvver_cameras_exception
{
  open_camera_error(const std::string& message="");
};

struct camera_parameter_error: public virtual silvver_cameras_exception
{
  camera_parameter_error(const std::string& message="");
};

struct file_parsing_error: public virtual silvver_cameras_exception
{
  file_parsing_error(const std::string& message="");
};

struct missing_field: public virtual file_parsing_error
{
  missing_field(const std::string& message="")
    :file_parsing_error(message)
  {}
};

struct type_error: public virtual file_parsing_error
{
  type_error(const std::string& message="")
    :file_parsing_error(message)
  {}
};


std::string errorsInfo2string(const silvver_cameras_exception& e);


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

typedef boost::error_info<struct tag_fieldName, std::string>
  info_fieldName;

typedef boost::error_info<struct tag_arrayIndex, int>
  info_arrayIndex;


#endif /* _EXCEPTIONS_HPP_ */
