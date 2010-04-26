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

#ifndef _SILVVER_EXCEPTIONS_HPP_
#define _SILVVER_EXCEPTIONS_HPP_

namespace silvver
{
  class time_expired_error: public std::runtime_error
  {
  public:
    time_expired_error(const std::string& whatArg)
      :runtime_error(whatArg){}
  };

  class connection_error: public std::runtime_error
  {
  public:
    connection_error(const std::string& whatArg)
      :runtime_error(whatArg){}
  };
}

#endif /* _SILVVER_EXCEPTIONS_HPP_ */
