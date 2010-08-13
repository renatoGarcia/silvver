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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include <boost/scoped_ptr.hpp>
#include <string>

#include "channel.hpp"
#include "channelTypes.hpp"
#include "exceptions.hpp"

class Connection
{
public:
  /** Default constructor.
   * Only construct a Connection, don't wait for a cliente or connector to a
   * server.  */
  Connection();

  ~Connection();

  /** Act as a server, waiting for a cliente connection request.
   *
   * @param receptionistEp The tcp/ip endpoint of the receptionist. */
  void waitConnection(const TcpIpEp& receptionistEp);

  /** Act as a client, connecting to a server.
   *
   * @param receptionistEp The tcp/ip endpoint of the server receptionist.
   * @param channelType The channel type used in connection.  */
  void connect(const TcpIpEp& receptionistEp, ChannelsEnum channelType);

  /// Close the current connection.
  void close();

  template <class T>
  void send(const T& t);

  template <class T>
  void receive(T& t);

private:
  template <class T>
  void serialize(std::string& data, const T& t);

  template <class T>
  void deserialize(T& t, const std::string& data);

  boost::scoped_ptr<Channel> channel;
};

#endif /* _CONNECTION_HPP_ */
