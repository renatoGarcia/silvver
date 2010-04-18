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

#ifndef _SERIALIZATIONS_HPP_
#define _SERIALIZATIONS_HPP_

#include "silvverTypes.hpp"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/array.hpp>

namespace boost
{
  namespace serialization
  {
    template<class Archive>
    void
    serialize(Archive& ar, silvver::Position& position, const unsigned version)
    {
      ar & position.x;
      ar & position.y;
      ar & position.z;
    }

    template<class Archive>
    void
    serialize(Archive& ar, silvver::Pose& pose, const unsigned version)
    {
      ar & boost::serialization::base_object<silvver::Position>(pose);
      ar & pose.rotationMatrix;
    }

    template<class Archive, class T>
    void
    serialize(Archive& ar, silvver::Identity<T>& id, const unsigned version)
    {
      ar & boost::serialization::base_object<T>(id);
      ar & id.uid;
      ar & id.timestamp;
    }
  } // namespace serialization
} // namespace boost

#endif // _SERIALIZATIONS_HPP_
