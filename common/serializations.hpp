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

#include <boost/serialization/array.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/split_free.hpp>

#include "silvverTypes.hpp"
#include "silvverImage.hpp"

namespace boost
{
  namespace serialization
  {
    template<class Archive>
    void
    serialize(Archive& ar, silvver::AbstractCameraUid& uid,
              const unsigned version)
    {
      ar & uid.targetSystem;
      ar & uid.hardCamera;
    }

    template<class Archive>
    void
    serialize(Archive& ar, silvver::TargetUid& uid,
              const unsigned version)
    {
      ar & uid.targetSystem;
      ar & uid.internal;
    }

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
    }

    template<class Archive>
    void
    save(Archive& ar, const silvver::Image& image, const unsigned int version)
    {
      ar << image.nChannels;
      ar << image.depth;
      ar << image.dataOrder;
      ar << image.origin;
      ar << image.width;
      ar << image.height;
      ar << image.imageSize;
      for(int i=0; i<image.imageSize; ++i)
      {
        ar << image.imageData[i];
      }
      ar << image.widthStep;
    }

    template<class Archive>
    void
    load(Archive& ar, silvver::Image& image, const unsigned int version)
    {
      int old_size = image.imageSize;

      ar >> image.nChannels;
      ar >> image.depth;
      ar >> image.dataOrder;
      ar >> image.origin;
      ar >> image.width;
      ar >> image.height;
      ar >> image.imageSize;
      if(image.imageSize != old_size)
      {
        delete[] image.imageData;
        image.imageData = new char[image.imageSize];
      }
      for(int i=0; i<image.imageSize ; ++i)
      {
        ar >> image.imageData[i];
      }
      ar >> image.widthStep;

      image.imageDataOrigin = image.imageData;
    }

    template<class Archive, class T>
    void
    serialize(Archive& ar,  silvver::CameraReading<T>& id,
              const unsigned version)
    {
      ar & id.camUid;
      ar & id.timestamp;
      ar & id.image;
      ar & id.localizations;
    }
  } // namespace serialization
} // namespace boost

BOOST_SERIALIZATION_SPLIT_FREE(silvver::Image)

#endif // _SERIALIZATIONS_HPP_
