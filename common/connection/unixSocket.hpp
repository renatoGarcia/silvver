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

#ifndef _UNIX_SOCKET_HPP_
#define _UNIX_SOCKET_HPP_

#include <boost/asio/local/stream_protocol.hpp>

#include "asioStream.hpp"
#include "channelTypes.hpp"

struct UnixSocketSpec
{
  typedef boost::asio::local::stream_protocol AsioClass;
  static const ChannelType portableClassId = UNIX_SOCKET;
};

typedef AsioStream<UnixSocketSpec> UnixSocket;

#endif /* _UNIX_SOCKET_HPP_ */
