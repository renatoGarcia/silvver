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

#include "mountedTarget.hpp"

MountedTarget::MountedTarget(const boost::array<double,3>& bodyTranslation,
                             const boost::array<double,9>& bodyRotation)
  :bodyTranslation(bodyTranslation)
  ,bodyRotation(bodyRotation)
{}

MountedTarget::~MountedTarget()
{}

void
MountedTarget::localizeBody(silvver::Pose& pose) const
{
  silvver::Pose tempPose(pose);

  //------------------------------- Rbc = Rtc x Rbt
  // Rotation of body in camera coordinates is Rotation of target in camera
  // coordinates vec rotation of body in target coordinates
  pose.rotationMatrix[0] =
    tempPose.rotationMatrix[0] * this->bodyRotation[0] +
    tempPose.rotationMatrix[1] * this->bodyRotation[3] +
    tempPose.rotationMatrix[2] * this->bodyRotation[6];
  pose.rotationMatrix[1] =
    tempPose.rotationMatrix[0] * this->bodyRotation[1] +
    tempPose.rotationMatrix[1] * this->bodyRotation[4] +
    tempPose.rotationMatrix[2] * this->bodyRotation[7];
  pose.rotationMatrix[2] =
    tempPose.rotationMatrix[0] * this->bodyRotation[2] +
    tempPose.rotationMatrix[1] * this->bodyRotation[5] +
    tempPose.rotationMatrix[2] * this->bodyRotation[8];
  pose.rotationMatrix[3] =
    tempPose.rotationMatrix[3] * this->bodyRotation[0] +
    tempPose.rotationMatrix[4] * this->bodyRotation[3] +
    tempPose.rotationMatrix[5] * this->bodyRotation[6];
  pose.rotationMatrix[4] =
    tempPose.rotationMatrix[3] * this->bodyRotation[1] +
    tempPose.rotationMatrix[4] * this->bodyRotation[4] +
    tempPose.rotationMatrix[5] * this->bodyRotation[7];
  pose.rotationMatrix[5] =
    tempPose.rotationMatrix[3] * this->bodyRotation[2] +
    tempPose.rotationMatrix[4] * this->bodyRotation[5] +
    tempPose.rotationMatrix[5] * this->bodyRotation[8];
  pose.rotationMatrix[6] =
    tempPose.rotationMatrix[6] * this->bodyRotation[0] +
    tempPose.rotationMatrix[7] * this->bodyRotation[3] +
    tempPose.rotationMatrix[8] * this->bodyRotation[6];
  pose.rotationMatrix[7] =
    tempPose.rotationMatrix[6] * this->bodyRotation[1] +
    tempPose.rotationMatrix[7] * this->bodyRotation[4] +
    tempPose.rotationMatrix[8] * this->bodyRotation[7];
  pose.rotationMatrix[8] =
    tempPose.rotationMatrix[6] * this->bodyRotation[2] +
    tempPose.rotationMatrix[7] * this->bodyRotation[5] +
    tempPose.rotationMatrix[8] * this->bodyRotation[8];

  //------------------------------- Tbc = Ttc + Rtc x Tbt
  // Translation of body in camera coordinates is translation of target in
  // camera coordinates plus rotation of target in camera coordinates vec
  // translation of body in target coordinates
  pose.x = tempPose.x +
    tempPose.rotationMatrix[0] * this->bodyTranslation[0] +
    tempPose.rotationMatrix[1] * this->bodyTranslation[1] +
    tempPose.rotationMatrix[2] * this->bodyTranslation[2];
  pose.y = tempPose.y +
    tempPose.rotationMatrix[3] * this->bodyTranslation[0] +
    tempPose.rotationMatrix[4] * this->bodyTranslation[1] +
    tempPose.rotationMatrix[5] * this->bodyTranslation[2];
  pose.z = tempPose.z +
    tempPose.rotationMatrix[6] * this->bodyTranslation[0] +
    tempPose.rotationMatrix[7] * this->bodyTranslation[1] +
    tempPose.rotationMatrix[8] * this->bodyTranslation[2];
}
