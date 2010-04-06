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

#ifndef _MOUNTED_TARGET_HPP_
#define _MOUNTED_TARGET_HPP_

#include <boost/array.hpp>

#include "silvverTypes.hpp"

class MountedTarget
{
protected:

  /** Class constructor.
   * @param bodyTranslation Translation of body center in target coordinate
                            system [x,y,z].
   * @param bodyRotation Rotation matrix of body in target coordinate system
                         [r11, r12, r13, r21, r22, r23, r31, r32, r33].   */
  MountedTarget(const boost::array<double,3>& bodyTranslation,
                const boost::array<double,9>& bodyRotation);

  virtual ~MountedTarget();

  /** Convert the target pose to body pose.
   * @param pose The pose that will be converted. */
  void localizeBody(silvver::Pose& pose) const;

private:
  /** Translation of body center in target coordinate system (Tbt).
   * The order is [x,y,z] */
  const boost::array<double,3> bodyTranslation;

  /** Rotation matrix of body in target coordinate system (Rbt).
   * The order of items is: r11, r12, r13, r21, r22, r23, r31, r32, r33 */
  const boost::array<double,9> bodyRotation;

};

#endif /* _MOUNTED_TARGET_HPP_ */
