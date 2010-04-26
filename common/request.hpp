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

#ifndef _REQUEST_HPP_
#define _REQUEST_HPP_

/** \file request.hpp
 * \brief Types for request actions from receptionist on silvver-server.
 */

#include <boost/variant.hpp>
#include <boost/serialization/variant.hpp>

#include "processorOptions.hpp"

/// This type symbolize a uninitialized Request.
struct NullRequest
{
  template<typename Archive>
  void serialize(Archive& ar, const unsigned)
  {}
};

struct AddTargetClient
{
  unsigned targetId;
  unsigned short localPort;

  AddTargetClient()
    :targetId(0)
    ,localPort(0)
  {}

  AddTargetClient(unsigned targetId, unsigned short localPort)
    :targetId(targetId)
    ,localPort(localPort)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned)
  {
    ar & targetId;
    ar & localPort;
  }
};

struct DelTargetClient
{
  unsigned targetId;
  unsigned short localPort;

  DelTargetClient()
    :targetId(0)
    ,localPort(0)
  {}

  DelTargetClient(unsigned targetId, unsigned short localPort)
    :targetId(targetId)
    ,localPort(localPort)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned)
  {
    ar & targetId;
    ar & localPort;
  }
};

struct AddCameraClient
{
  std::string cameraUid;
  unsigned short localPort;

  AddCameraClient()
    :cameraUid()
    ,localPort(0)
  {}

  AddCameraClient(std::string cameraUid, unsigned short localPort)
    :cameraUid(cameraUid)
    ,localPort(localPort)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned)
  {
    ar & cameraUid;
    ar & localPort;
  }
};

struct DelCameraClient
{
  std::string cameraUid;
  unsigned short localPort;

  DelCameraClient()
    :cameraUid()
    ,localPort(0)
  {}

  DelCameraClient(std::string cameraUid, unsigned short localPort)
    :cameraUid(cameraUid)
    ,localPort(localPort)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned)
  {
    ar & cameraUid;
    ar & localPort;
  }
};

struct AddCamera
{
  procOpt::AnyProcOpt processorOpt;
  unsigned short localPort;
  std::string cameraUid;

  AddCamera()
    :processorOpt()
    ,localPort(0)
    ,cameraUid()
  {}

  AddCamera(const procOpt::AnyProcOpt& processorOpt, unsigned short localPort,
            const std::string& cameraUid)
    :processorOpt(processorOpt)
    ,localPort(localPort)
    ,cameraUid(cameraUid)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned)
  {
    ar & processorOpt;
    ar & localPort;
    ar & cameraUid;
  }
};

struct DelCamera
{
  std::string cameraUid;

  DelCamera()
    :cameraUid()
  {}

  DelCamera(const std::string& cameraUid)
    :cameraUid(cameraUid)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned)
  {
    ar & cameraUid;
  }
};

typedef boost::variant<NullRequest,
                       AddTargetClient,
                       DelTargetClient,
                       AddCameraClient,
                       DelCameraClient,
                       AddCamera,
                       DelCamera       > Request;

#endif // _REQUEST_HPP_
