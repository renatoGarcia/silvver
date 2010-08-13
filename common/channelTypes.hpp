/* Copyright 2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#ifndef _CHANNEL_TYPES_HPP_
#define _CHANNEL_TYPES_HPP_

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/string.hpp>
#include <boost/variant.hpp>

enum ChannelsEnum
{
  TCP_IP,
  IPC
};

struct TcpIpEp
{
  boost::asio::ip::tcp::endpoint ep;

  TcpIpEp()
    :ep()
  {}

  TcpIpEp(const boost::asio::ip::tcp::endpoint& ep)
    :ep(ep)
  {}

  TcpIpEp(const std::string& ipAddress, const unsigned short portNumber)
    :ep(boost::asio::ip::address::from_string(ipAddress), portNumber)
  {}

  template<class Archive>
  void save(Archive & ar, const unsigned int version) const
  {
    std::string s = this->ep.address().to_string();
    unsigned int i = this->ep.port();
    ar << s;
    ar << i;
  }

  template<class Archive>
  void load(Archive & ar, const unsigned int version)
  {
    std::string ipAddress;
    unsigned short portNumber;

    ar >> ipAddress;
    ar >> portNumber;

    this->ep = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ipAddress), portNumber);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER();
};

struct IpcEp
{
  std::string ep;

  IpcEp()
    :ep()
  {}

  IpcEp(const std::string& ep)
    :ep(ep)
  {}

  template<class Archive>
  void serialize(Archive& ar, const unsigned version)
  {
    ar & ep;
  }
};

typedef boost::variant<TcpIpEp, IpcEp> Endpoint;

#endif /* _CHANNEL_TYPES_HPP_ */
