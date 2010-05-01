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

#include "exceptions.hpp"

#include <sstream>

std::string errorsInfo2string(const silvver_cameras_exception& e)
{
  std::ostringstream ostr;
  if (const std::string* mi = boost::get_error_info<info_what>(e))
    ostr << "What: " << *mi << std::endl;
  if (const std::string* mi = boost::get_error_info<info_cameraModel>(e))
    ostr << "Camera model: " << *mi << std::endl;
  if (const unsigned* mi = boost::get_error_info<info_cameraUid>(e))
    ostr << "Camera UID: " << *mi << std::endl;
  if (const std::string* mi = boost::get_error_info<info_colorMode>(e))
    ostr << "Color mode: " << *mi << std::endl;
  if (const boost::array<unsigned, 2>* mi = boost::get_error_info<info_resolution>(e))
    ostr << "Resolution: " << mi->at(0) << " x " << mi->at(1) << std::endl;
  if (const float* mi = boost::get_error_info<info_frameRate>(e))
    ostr << "Frame rate: " << *mi << std::endl;
  if (const std::string* mi = boost::get_error_info<info_featureName>(e))
    ostr << "Feature: " << *mi << std::endl;
  if (const std::string* mi = boost::get_error_info<info_featureValue>(e))
    ostr << "Feature value: " << *mi << std::endl;
  if (const std::string* mi = boost::get_error_info<info_featureRange>(e))
    ostr << "Feature range: " << *mi << std::endl;
  if (const std::string* mi = boost::get_error_info<boost::errinfo_file_name>(e))
    ostr << "File name: " << *mi << std::endl;
  if (const std::string* mi = boost::get_error_info<info_bayer>(e))
    ostr << "Bayer method: " << *mi << std::endl;
  if (const std::string* mi = boost::get_error_info<info_colorFilter>(e))
    ostr << "Color filter: " << *mi << std::endl;

  if (const int* mi = boost::get_error_info<info_cameraIndex>(e))
    ostr << "Camera index: " << *mi << std::endl;
  if (const int* mi = boost::get_error_info<info_targetIndex>(e))
    ostr << "Target index: " << *mi << std::endl;
  if (const std::string* mi = boost::get_error_info<info_fieldName>(e))
    ostr << "Field name: " << *mi << std::endl;
  if (const int* mi = boost::get_error_info<info_arrayIndex>(e))
    ostr << "Array index: " << *mi << std::endl;


  return ostr.str();
}

char const *
silvver_cameras_exception::what() const throw()
{
  if (const std::string* mi = boost::get_error_info<info_what>(*this))
  {
    return mi->c_str();
  }
  else
  {
    return "";
  }
}
