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

#ifndef _SILVVER_VERSION_HPP_
#define _SILVVER_VERSION_HPP_

//  SILVVER_VERSION % 100 is the patch level
//  SILVVER_VERSION / 100 % 1000 is the minor version
//  SILVVER_VERSION / 100000 is the major version
#define SILVVER_VERSION 500

//  SILVVER_LIB_VERSION must be defined to be the same as SILVVER_VERSION
//  but as a *string* in the form "x_y[_z]" where x is the major version
//  number, y is the minor version number, and z is the patch level if not 0.
#define SILVVER_LIB_VERSION "0_5"

#endif /* _SILVVER_VERSION_HPP_ */
