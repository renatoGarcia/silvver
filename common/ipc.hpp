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

#ifndef _IPC_HPP_
#define _IPC_HPP_

#include <boost/array.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <string>

#include "channel.hpp"
#include "channelTypes.hpp"
#include "exceptions.hpp"

class Ipc
  :public Channel
{
public:
  /// Construct a server side ipc channel.
  Ipc();

  /** Construct a client side ipc channel.
   * Construct a new ipc and connect it to server.
   * @param ipcEp The endpoint of ipc server where to connect.  */
  Ipc(const IpcEp& ipcEp);

  virtual ~Ipc() throw();

  virtual void close();

  virtual Endpoint getEndpoint() const;

  virtual void send(const std::string& data);

  virtual void receive(std::string& data);

private:
  static int uidCounter;

  static std::string genMQName();

private:
  /// Check connectivity between peers (server side).
  void heartBeatServer();

  /// Check connectivity between peers (client side).
  void heartBeatClient();

  boost::array<char, 1024> inBuffer;

  boost::scoped_ptr<boost::interprocess::message_queue> messageQueue;

  const std::string mqName;

  boost::scoped_ptr<boost::thread> hbThread;

  bool connectionIsBroken;
};

#endif /* _IPC_HPP_ */
