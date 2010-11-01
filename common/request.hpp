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
#include "silvverTypes.hpp"

/// This type symbolize a uninitialized Request.
struct NullRequest
{
  template<typename Archive>
  void serialize(Archive& ar, const unsigned)
  {}
};

struct AddTargetClient
{
  silvver::TargetUid targetUid;

  AddTargetClient()
    :targetUid()
  {}

  AddTargetClient(silvver::TargetUid targetUid)
    :targetUid(targetUid)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned)
  {
    ar & targetUid;
  }
};

struct AddCameraClient
{
  silvver::AbstractCameraUid cameraUid;

  AddCameraClient()
    :cameraUid()
  {}

  AddCameraClient(const silvver::AbstractCameraUid& cameraUid)
    :cameraUid(cameraUid)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned)
  {
    ar & cameraUid;
  }
};

struct AddTargetSetClient
{
  silvver::TargetSetUid targetSetUid;

  AddTargetSetClient()
    :targetSetUid()
  {}

  AddTargetSetClient(silvver::TargetSetUid targetSetUid)
    :targetSetUid(targetSetUid)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned)
  {
    ar & targetSetUid;
  }
};

struct AddCamera
{
  procOpt::AnyProcOpt processorOpt;
  silvver::AbstractCameraUid cameraUid;

  AddCamera()
    :processorOpt()
    ,cameraUid()
  {}

  AddCamera(const procOpt::AnyProcOpt& processorOpt,
            const silvver::AbstractCameraUid& cameraUid)
    :processorOpt(processorOpt)
    ,cameraUid(cameraUid)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned)
  {
    ar & processorOpt;
    ar & cameraUid;
  }
};

typedef boost::variant<NullRequest,
                       AddTargetClient,
                       AddCameraClient,
                       AddTargetSetClient,
                       AddCamera          > Request;

#endif // _REQUEST_HPP_
