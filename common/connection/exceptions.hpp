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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _COMMON_EXCEPTIONS_HPP_
#define _COMMON_EXCEPTIONS_HPP_

#include <stdexcept>
#include <string>

#include "boost/enum.hpp"

namespace connection {

BOOST_ENUM(error_code,
           (success)
           (connection_error)
           (broken_connection)
           (data_error));

} // namespace connection

class connection_error
  :public std::runtime_error
{
public:
  connection_error(const std::string& whatArg);
  virtual ~connection_error() throw();
};

class broken_connection
  :public connection_error
{
public:
  broken_connection(const std::string& whatArg);
};

class data_error
  :public connection_error
{
public:
  data_error(const std::string& whatArg);
};

#endif /* _COMMON_EXCEPTIONS_HPP_ */
