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

#ifndef _CLIENTS_MAP_H_
#define _CLIENTS_MAP_H_

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include "ioConnection.hpp"

/** A class to hold the connected clients.
 * This class wrap the STL multimap to thread safety.
 */
class ClientsMap
{
public:

  /** Instantiate or get the already created instance of the class.
   * The ClientsMap class is singleton, this method allow instantiate it.
   *
   * @return A shared pointer to the instance.
   */
  static boost::shared_ptr<ClientsMap> instantiate();

  /** Add a client.
   *
   * @param request An AddOutput object with the description of the client.
   * @param outputConnection An IoConnection already connected with the client.
   */
  void addOutput(unsigned targetId,
                 boost::shared_ptr<IoConnection> outputConnection);

  /** Delete a client
   *
   * @param idTarget The id of target which the client is listening.
   * @param remotePort The remote port where the client is connected, given by
   *                   IoConnection::getRemotePort()
   */
  void delOutput(unsigned idTarget, unsigned remotePort);

  /** Return connections to all clients which are listening for a given target.
   *
   * @param idTarget The id of desired target.
   * @return A vector of shared_prt with IoConnection to all clients found.
   */
  std::vector< boost::shared_ptr<IoConnection> > findClient(unsigned idTarget);

private:

  typedef std::multimap< unsigned, boost::shared_ptr<IoConnection> > TMultiMap;

  static boost::shared_ptr<ClientsMap> singleInstance;
  static boost::mutex instantiatingMutex;

  boost::mutex accessMap;

  /// A dummy private constructor.
  ClientsMap();

  /// The key of multimap is the id of target
  TMultiMap client;

};

#endif /* _CLIENTS_MAP_H_ */
