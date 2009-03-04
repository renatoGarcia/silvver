#ifndef REQUEST_HPP
#define REQUEST_HPP

/** \file request.hpp
 * \brief Types for request actions from receptionist on silver-server.
 */

#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>

#include "targetType.hpp"

/// This type symbolize a uninitialized Request.
struct NullRequest
{
  template<typename Archive>
  void serialize(Archive& ar, const unsigned version)
  {}
};

struct AddOutput
{
  unsigned targetId;
  unsigned short localPort;

  AddOutput()
    :targetId(0)
    ,localPort(0)
  {}

  AddOutput(unsigned targetId, unsigned short localPort)
    :targetId(targetId)
    ,localPort(localPort)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned version)
  {
    ar & targetId;
    ar & localPort;
  }
};

struct DelOutput
{
  unsigned targetId;
  unsigned short localPort;

  DelOutput()
    :targetId(0)
    ,localPort(0)
  {}

  DelOutput(unsigned targetId, unsigned short localPort)
    :targetId(targetId)
    ,localPort(localPort)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned version)
  {
    ar & targetId;
    ar & localPort;
  }
};

struct AddCamera
{
  TargetType targetType;
  unsigned short localPort;

  AddCamera()
    :targetType()
    ,localPort(0)
  {}

  AddCamera(TargetType targetType, unsigned short localPort)
    :targetType(targetType)
    ,localPort(localPort)
  {}

  AddCamera(const std::string& targetType, unsigned short localPort)
    :targetType(targetType)
    ,localPort(localPort)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned version)
  {
    ar & targetType;
    ar & localPort;
  }
};

struct DelCamera
{
  unsigned short localPort;

  DelCamera()
    :localPort(0)
  {}

  DelCamera(unsigned short localPort)
    :localPort(localPort)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned version)
  {
    ar & localPort;
  }
};

namespace boost
{
  namespace serialization
  {
    template<typename Archive>
    void
    serialize(Archive& ar, TargetType& targetType, const unsigned version)
    {
      ar & targetType.state;
    }
  }
}



typedef boost::variant<NullRequest,
                       AddOutput,
                       DelOutput,
                       AddCamera,
                       DelCamera> Request;

#endif // REQUEST_HPP
