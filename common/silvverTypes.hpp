/* Copyright 2009-2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

/**
 * @file silvverTypes.hpp
 *
 * @brief Types used to represents the localized targets.
 */
#ifndef _SILVVER_TYPES_HPP_
#define _SILVVER_TYPES_HPP_

#include <boost/array.hpp>
#include <boost/operators.hpp>
#include <boost/tuple/tuple.hpp>
#include <cmath>
#include <cstddef>
#include <stdint.h>
#include <vector>

namespace silvver
{
  //------------------------------ Silvver UIDs
  typedef unsigned TargetSetUid;

  struct AbstractCameraUid
  {
    TargetSetUid targetSet;
    unsigned hardCamera;

    AbstractCameraUid()
      :targetSet(0)
      ,hardCamera(0)
    {}

    AbstractCameraUid(const TargetSetUid& targetSet, const unsigned hardCamera)
      :targetSet(targetSet)
      ,hardCamera(hardCamera)
    {}

    AbstractCameraUid(const AbstractCameraUid& uid)
      :targetSet(uid.targetSet)
      ,hardCamera(uid.hardCamera)
    {}

    bool
    operator<(const AbstractCameraUid& uid) const
    {
      if (this->targetSet < uid.targetSet) return true;
      else if (this->targetSet > uid.targetSet) return false;

      // If here, the two targetSetUids are equals
      else if (this->hardCamera < uid.hardCamera) return true;
      else if (this->hardCamera > uid.hardCamera) return false;

      // If here, the two AbstractCameraUids are equals
      else return false;
    }

    bool
    operator==(const AbstractCameraUid& uid) const
    {
      return ((this->targetSet == uid.targetSet) &&
              (this->hardCamera == uid.hardCamera));
    }
  };

  template <class charT, class traits>
  inline
  std::basic_ostream<charT,traits>&
  operator<<(std::basic_ostream<charT,traits>& strm,
             const AbstractCameraUid& uid)
  {
    strm << "(" << uid.targetSet << ", " << uid.hardCamera << ')';
    return strm;
  }

  struct TargetUid
  {
    TargetSetUid targetSet;
    unsigned internal;

    TargetUid()
      :targetSet(0)
      ,internal(0)
    {}

    TargetUid(const TargetSetUid& targetSet, const unsigned internal)
      :targetSet(targetSet)
      ,internal(internal)
    {}

    TargetUid(const TargetUid& uid)
      :targetSet(uid.targetSet)
      ,internal(uid.internal)
    {}

    bool
    operator<(const TargetUid& uid) const
    {
      if (this->targetSet < uid.targetSet) return true;
      else if (this->targetSet > uid.targetSet) return false;

      // If here, the two targetSetUids are equals
      else if (this->internal < uid.internal) return true;
      else if (this->internal > uid.internal) return false;

      // If here, the two TargetUids are equals
      else return false;
    }

    bool
    operator==(const TargetUid& uid) const
    {
      return ((this->targetSet == uid.targetSet) &&
              (this->internal == uid.internal));
    }
  };

  template <class charT, class traits>
  inline
  std::basic_ostream<charT,traits>&
  operator<<(std::basic_ostream<charT,traits>& strm,
             const TargetUid& uid)
  {
    strm << "(" << uid.targetSet << ", " << uid.internal << ')';
    return strm;
  }

  //------------------------------ Position

  struct Position
  {
    double x;
    double y;
    double z;

    Position(const double x=0.0, const double y=0.0, const double z=0.0)
      :x(x)
      ,y(y)
      ,z(z)
    {}

    Position(const Position& position)
      :x(position.x)
      ,y(position.y)
      ,z(position.z)
    {}

    Position&
    operator=(const Position& position)
    {
      if (this == &position)
      {
        return *this;
      }

      this->x = position.x;
      this->y = position.y;
      this->z = position.z;
      return *this;
    }
  };

  template <class charT, class traits>
  inline
  std::basic_ostream<charT,traits>&
  operator<<(std::basic_ostream<charT,traits>& strm,
             const Position& position)
  {
    strm << position.x << '\t' << position.y << '\t' << position.z;
    return strm;
  }

  //------------------------------ Pose
  struct Pose
    :public Position
  {
    /// The order of items is: r11, r12, r13, r21, r22, r23, r31, r32, r33
    boost::array<double, 9> rotationMatrix;

    double& rotMat(int row, int column)
    {
      return this->rotationMatrix.at((3*row)+column);
    }

    /** Calculates the theta angle. [rad]
     * The theta is the angle between the x origin's axis and the projection
     * of x axis of this pose in x,y origin's plane.
     *
     * @return An angle in range [-pi, pi]
     */
    double theta()
    {
      return atan2(rotationMatrix[3], rotationMatrix[0]);
    }

    Pose()
      :Position()
    {}

    Pose(const Pose& pose)
      :Position(pose)
      ,rotationMatrix(pose.rotationMatrix)
    {}

    Pose&
    operator=(const Pose& pose)
    {
      if (this == &pose)
      {
        return *this;
      }

      Position::operator=(pose);
      this->rotationMatrix = pose.rotationMatrix;
      return *this;
    }
  };

  template<class charT, class traits>
  inline
  std::basic_ostream<charT,traits>&
  operator<<(std::basic_ostream<charT,traits>& strm,
             const Pose& pose)
  {
    strm << static_cast<Position>(pose);
    for (int i = 0; i < 9; ++i)
    {
      strm  << '\t' << pose.rotationMatrix[i];
    }

    return strm;
  }

  //------------------------------ Identity
  template<class BaseClass>
  struct Identity
    :public BaseClass
  {
    /// UID of this target;
    TargetUid uid;

    Identity()
      :BaseClass()
      ,uid()
    {}

    Identity(const BaseClass& base, const TargetUid& uid)
      :BaseClass(base)
      ,uid(uid)
    {}

    Identity<BaseClass>&
    operator=(const Identity<BaseClass>& identity)
    {
      if (this == &identity)
      {
        return *this;
      }

      BaseClass::operator=(identity);
      this->uid = identity.uid;
      return *this;
    }
  };

  template <class charT, class traits, class BaseClass>
  inline
  std::basic_ostream<charT,traits>&
  operator << (std::basic_ostream<charT,traits>& strm,
               const Identity<BaseClass>& identity)
  {
    strm << identity.uid << '\t' << static_cast<BaseClass>(identity);
    return strm;
  }

  //------------------------------ Camera reading
  template<class TargetType>
  struct CameraReading
  {
    AbstractCameraUid uid;
    uint64_t timestamp;
    std::vector<Identity<TargetType> > localizations;

    CameraReading()
      :uid()
      ,timestamp(0)
      ,localizations()
    {}

    CameraReading(const AbstractCameraUid& uid,
                  uint64_t timestamp,
                  const std::vector<Identity<TargetType> >& localizations)
      :uid(uid)
      ,timestamp(timestamp)
      ,localizations(localizations)
    {}
  };
} // namespace silvver

#endif /* _SILVVER_TYPES_HPP_ */
