#ifndef SERIALIZATIONS_HPP
#define SERIALIZATIONS_HPP

#include <silverTypes.hpp>

#include <boost/serialization/base_object.hpp>

namespace boost
{
  namespace serialization
  {

    template<typename Archive>
    void
    serialize(Archive& ar, silver::Position& position, const unsigned version)
    {
      ar & position.x;
      ar & position.y;
    }

    template<typename Archive>
    void
    serialize(Archive& ar, silver::Pose& pose, const unsigned version)
    {
      ar & boost::serialization::base_object<silver::Position>(pose);
      ar & pose.theta;
    }

    template<typename Archive>
    void
    serialize(Archive& ar, silver::Ente& ente, const unsigned version)
    {
      ar & boost::serialization::base_object<silver::Pose>(ente);
      ar & ente.id;
      ar & ente.weigh;
    }

    template<typename Archive>
    void
    serialize(Archive& ar, silver::Blob& blob, const unsigned version)
    {
      ar & blob.highestVertex;
      ar & blob.lowestVertex;
    }

  } // namespace serialization
} // namespace boost

#endif // SERIALIZATIONS_HPP
