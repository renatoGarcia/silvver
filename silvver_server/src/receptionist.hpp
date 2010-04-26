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

#ifndef _RECEPTIONIST_HPP_
#define _RECEPTIONIST_HPP_

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <map>
#include <string>

#include "outputMultiMap.hpp"
#include "inputInterface.hpp"
#include "streamConnection.hpp"
#include "request.hpp"

/** Receive and manage the incoming connections demands.
 *
 * \author Renato Florentino Garcia
 */
class Receptionist
{
public:
  /** Necessary for boost::apply_visitor function to receive a Receptionist
   * object as an argument. With this typedef, the Receptionist class fulfills
   * the requirements of a "static visitor" boost::variant concept. */
  typedef void result_type;

public:
  /** The class constructor
   * @param localPort TCP port where the receptionist will be hearing for
   *                  incoming connections. */
  Receptionist(unsigned localPort);

  ~Receptionist();

  // Visitors of the boost::variant Request type.
  void operator()(NullRequest& request) const;
  void operator()(AddTargetClient& request) const;
  void operator()(DelTargetClient& request) const;
  void operator()(AddCameraClient& request) const;
  void operator()(DelCameraClient& request) const;
  void operator()(AddCamera& request);
  void operator()(DelCamera& request);

private:
  void run();

  void handleAccept(StreamConnection::pointer connection);

  void handleRead();

  boost::asio::io_service ioService;

  boost::asio::ip::tcp::acceptor acceptor;

  /// Handle the TCP connection with the client being currently managed.
  StreamConnection::pointer currentReception;

  /// Handle the request of the client being currently managed.
  Request request;

  /// Connected input clients collection
  std::map<std::string, boost::shared_ptr<InputInterface> > mapInputs;

  boost::shared_ptr<OutputMultiMap<CLIENT_TARGET, unsigned> > targetOutputs;

  boost::shared_ptr<OutputMultiMap<CLIENT_CAMERA, std::string> > cameraOutputs;

  /// Thread where the boost io_service will run
  boost::scoped_ptr<boost::thread> thReceptionist;
};

#endif /* _RECEPTIONIST_HPP_ */
