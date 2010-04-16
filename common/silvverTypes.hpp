/* Copyright 2009 Renato Florentino Garcia <fgar.renato@gmail.com>
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
#include <cmath>
#include <stdint.h>

namespace silvver
{
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
  operator << (std::basic_ostream<charT,traits>& strm,
               const Position& position)
  {
    strm << position.x << '\t' << position.y << '\t' << position.z;
    return strm;
  }

  //------------------------------ Pose
  struct Pose : public Position
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

  template <class charT, class traits>
  inline
  std::basic_ostream<charT,traits>&
  operator << (std::basic_ostream<charT,traits>& strm,
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
  struct Identity : public BaseClass
  {
    unsigned uid;
    uint64_t timestamp;

    Identity()
      :BaseClass()
      ,uid(0)
      ,timestamp(0)
    {}

    Identity(const BaseClass& base, const unsigned uid,
             const uint64_t timestamp)
      :BaseClass(base)
      ,uid(uid)
      ,timestamp(timestamp)
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
      this->timestamp = identity.timestamp;
      return *this;
    }
  };

  template <class charT, class traits, class BaseClass>
  inline
  std::basic_ostream<charT,traits>&
  operator << (std::basic_ostream<charT,traits>& strm,
               const Identity<BaseClass>& identity)
  {
    strm << identity.timestamp << '\t'
         << identity.uid << '\t' << static_cast<BaseClass>(identity);
    return strm;
  }

} // namespace silvver

#endif /* _SILVVER_TYPES_HPP_ */
