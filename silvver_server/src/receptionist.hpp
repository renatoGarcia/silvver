/* Copyright 2009-2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#ifndef _RECEPTIONIST_HPP_
#define _RECEPTIONIST_HPP_

#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <map>
#include <string>

#include "common/connection/acceptor.hpp"
#include "common/connection/channel.hpp"
#include "common/connection/tcpIp.hpp"
#include "common/request.hpp"
#include "common/silvverTypes.hpp"
#include "inputBase.hpp"
#include "clientMultiMap.hpp"

/// Receive and manage the incoming connections demands.
class Receptionist
{
public:
  /** Necessary for boost::apply_visitor function to receive a Receptionist
   * object as an argument. With this typedef, the Receptionist class fulfills
   * the requirements of a "static visitor" boost::variant concept.
   */
  typedef void result_type;

  /** The class constructor
   *
   * @param localPort TCP port where the receptionist will be hearing for
   *                  incoming connections.
   */
  Receptionist(unsigned localPort);

  ~Receptionist();

  // Visitors of the boost::variant Request type.
  void operator()(NullRequest& request);
  void operator()(AddTargetClient& request);
  void operator()(AddCameraClient& request);
  void operator()(AddTargetSetClient& request);
  void operator()(AddCamera& request);

private:
  void handleAccept(connection::Channel* channel, connection::error_code ec);

  void closeTargetClient(const silvver::TargetUid& targetUid,
                         boost::shared_ptr<connection::Channel> channel);

  void closeCameraClient(const silvver::AbstractCameraUid& cameraUid,
                         boost::shared_ptr<connection::Channel> channel);

  void closeTargetSetClient(const silvver::TargetSetUid& targetSetUid,
                            boost::shared_ptr<connection::Channel> channel);

  void closeCamera(const silvver::AbstractCameraUid& cameraUid);

  boost::asio::io_service ioService;

  connection::Acceptor<connection::TcpIp> acceptor;

  // /// Handle the TCP connection with the client being currently managed.
  boost::shared_ptr<connection::Channel> currentReception;

  /// AbstractCameras Connected.
  std::map<silvver::AbstractCameraUid, boost::shared_ptr<InputBase> > mapInputs;

  boost::shared_ptr<ClientMultiMap<silvver::TargetUid> > targetClients;

  boost::shared_ptr<ClientMultiMap<silvver::AbstractCameraUid> > abstractCameraClients;

  boost::shared_ptr<ClientMultiMap<silvver::TargetSetUid> > targetSetClients;

  /// Thread where the boost io_service will run
  boost::thread thReceptionist;
};

#endif /* _RECEPTIONIST_HPP_ */
