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

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/variant.hpp>

enum ChannelType
{
  TCP_IP,
  UNIX_SOCKET
};

typedef boost::asio::ip::tcp::endpoint TcpIpEp;
typedef boost::asio::local::stream_protocol::endpoint UnixSocketEp;

typedef boost::variant<TcpIpEp, UnixSocketEp> AnyEndpoint;

BOOST_SERIALIZATION_SPLIT_FREE(TcpIpEp);
BOOST_SERIALIZATION_SPLIT_FREE(UnixSocketEp);

namespace boost {
namespace serialization {

//-------------------------- TcpIpEp
template<class Archive>
void save(Archive& ar, const TcpIpEp& ep, const unsigned int version)
{
  std::string s(ep.address().to_string());
  unsigned int i(ep.port());
  ar << s;
  ar << i;
}

template<class Archive>
void load(Archive& ar, TcpIpEp& ep, const unsigned int version)
{
  std::string ipAddress;
  unsigned short portNumber;

  ar >> ipAddress;
  ar >> portNumber;

  ep = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ipAddress), portNumber);
}

//-------------------------- UnixSocketEp
template<class Archive>
void save(Archive& ar, const UnixSocketEp& ep, const unsigned int version)
{
  std::string path(ep.path());
  ar << path;
}

template<class Archive>
void load(Archive & ar, UnixSocketEp& ep, const unsigned int version)
{
  std::string path;
  ar >> path;
  ep = ::boost::asio::local::stream_protocol::endpoint(path);
}

} //namespace serialization
} //namespace boost

#endif /* _CHANNEL_TYPES_HPP_ */
