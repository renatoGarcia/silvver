#ifndef SERIALIZATIONS_HPP
#define SERIALIZATIONS_HPP

#include <silverTypes.hpp>

#include <boost/serialization/base_object.hpp>

namespace boost
{
  namespace serialization
  {

    template<class Archive>
    void
    serialize(Archive& ar, silver::Position& position, const unsigned version)
    {
      ar & position.x;
      ar & position.y;
      ar & position.z;
    }

    template<class Archive>
    void
    serialize(Archive& ar, silver::Pose& pose, const unsigned version)
    {
      ar & boost::serialization::base_object<silver::Position>(pose);
      // ar & pose.yaw;
      // ar & pose.pitch;
      // ar & pose.roll;
      for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
          ar & pose.rotationMatrix[i][j];
    }

    template<class Archive, class T>
    void
    serialize(Archive& ar, silver::Identity<T>& id, const unsigned version)
    {
      ar & boost::serialization::base_object<T>(id);
      ar & id.uid;
    }

//     template<class Archive>
//     void
//     serialize(Archive& ar, silver::Blob& blob, const unsigned version)
//     {
//       ar & blob.highestVertex;
//       ar & blob.lowestVertex;
//     }

  } // namespace serialization
} // namespace boost

#endif // SERIALIZATIONS_HPP
