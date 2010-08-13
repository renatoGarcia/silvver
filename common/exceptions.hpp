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

#ifndef _EXCEPTIONS_HPP_
#define _EXCEPTIONS_HPP_

#include <stdexcept>

class connection_error
  :public std::runtime_error
{
public:
  connection_error(const std::string& whatArg)
    :runtime_error(whatArg)
  {}

  virtual ~connection_error() throw()
  {}
};

class broken_connection
  :public connection_error
{
public:
  broken_connection(const std::string& whatArg)
    :connection_error(whatArg)
  {}
};

class data_error
  :public connection_error
{
public:
  data_error(const std::string& whatArg)
    :connection_error(whatArg)
  {}
};

#endif /* _EXCEPTIONS_HPP_ */
