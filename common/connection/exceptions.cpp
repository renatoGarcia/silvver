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

#include "exceptions.hpp"

connection_error:: connection_error(const std::string& whatArg)
  :runtime_error(whatArg)
{}

connection_error::~connection_error() throw()
{}

broken_connection::broken_connection(const std::string& whatArg)
  :connection_error(whatArg)
{}

data_error::data_error(const std::string& whatArg)
  :connection_error(whatArg)
{}

