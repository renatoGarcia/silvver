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

#ifndef REQUEST_HPP
#define REQUEST_HPP

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
  procOpt::AnyProcOpt processorOpt;
  unsigned short localPort;


  AddCamera()
    :processorOpt()
    ,localPort(0)
  {}

  AddCamera(const procOpt::AnyProcOpt& processorOpt, unsigned short localPort)
    :processorOpt(processorOpt)
    ,localPort(localPort)
  {}

  template<typename Archive>
  void serialize(Archive& ar, const unsigned version)
  {
    ar & processorOpt;
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

typedef boost::variant<NullRequest,
                       AddOutput,
                       DelOutput,
                       AddCamera,
                       DelCamera> Request;

#endif // REQUEST_HPP
