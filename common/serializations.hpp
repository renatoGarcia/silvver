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

#include <boost/serialization/array.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/split_free.hpp>
// #include <boost/serialization/export.hpp>

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
    }

    /** Serialization for IplImage.
     * Variables that are assigned to NULL are the ones ignored by OpenCV
     * (but used in Intel Ipl Image Library)
     * Code grabed from:
     * http://cycabtk.gforge.inria.fr/wiki/doku.php?id=howto:devel-hugr*/
    template<class Archive>
    void
    save(Archive& ar, const IplImage& image, const unsigned int version)
    {
      ar & image.nSize;
      ar & image.ID;
      ar & image.nChannels;
      ar & image.depth;
      ar & image.dataOrder;
      ar & image.origin;
      ar & image.width;
      ar & image.height;

      ar & image.imageSize;

      for(int i=0; i<image.imageSize ; i++)
        ar & image.imageData[i];

      ar & image.widthStep;
    }

    template<class Archive>
    void
    load(Archive& ar, IplImage& image, const unsigned int version)
    {
      int old_size = image.imageSize;

      ar & image.nSize;
      ar & image.ID;
      ar & image.nChannels;
      ar & image.depth;
      ar & image.dataOrder;
      ar & image.origin;
      ar & image.width;
      ar & image.height;

      image.roi = NULL;
      image.maskROI = NULL;

      ar & image.imageSize;
      if(image.imageSize != old_size)
      {
        delete image.imageData;
        image.imageData = new char[image.imageSize];
      }

      for(int i=0; i<image.imageSize ; i++)
      {
        ar & image.imageData[i];
      }

      ar & image.widthStep;

      bzero(image.BorderMode, 4);
      bzero(image.BorderConst, 4);

      image.imageDataOrigin = image.imageData;
    }

    template<class Archive, class T>
    void
    serialize(Archive& ar,  silvver::CameraReading<T>& id,
              const unsigned version)
    {
      ar & id.abstractCameraUid;
      ar & id.timestamp;
      ar & id.image;
      ar & id.localizations;
    }
  } // namespace serialization
} // namespace boost

// BOOST_CLASS_EXPORT_GUID(IplImage, "IplImage")
BOOST_SERIALIZATION_SPLIT_FREE(IplImage)

#endif // _SERIALIZATIONS_HPP_
