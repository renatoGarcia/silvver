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

#ifndef _LOG_HPP_
#define _LOG_HPP_

#include <boost/enum.hpp>
#include <logging.hpp>
#include <tsostream.hpp>

BOOST_ENUM(LogLevel,
           (TRACE)
           (INFO)
           (WARN)
           (ERROR)
           (NOTHING));

extern logging message;

#endif /* _LOG_HPP_ */
