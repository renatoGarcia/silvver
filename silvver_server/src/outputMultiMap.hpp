/* Copyright 2009, 2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#ifndef _OUTPUT_MULTIMAP_HPP_
#define _OUTPUT_MULTIMAP_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <map>
#include <vector>

#include "ioConnection.hpp"
#include "singleton.hpp"

enum ClientType
{
  CLIENT_TARGET,
  CLIENT_CAMERA
};

/** A class to hold the connected outputs.
 * This class wrap the STL multimap for thread safety.
 */
template<ClientType clientType, class KeyType>
class OutputMultiMap: public Singleton<OutputMultiMap<clientType, KeyType> >
{
public:

  /** Add a output.
   * @param silvverUid The silvver uid of target or camera being observed.
   * @param outputConnection An IoConnection already made with the output. */
  void addOutput(KeyType silvverUid,
                 boost::shared_ptr<IoConnection> outputConnection);

  /** Delete a output.
   * @param silvverUid The silvver uid of target or camera being observed.
   * @param remotePort The remote port where the client is connected, given by
   *                   IoConnection::getRemotePort()  */
  void delOutput(KeyType silvverUid, unsigned remotePort);

  /** Return connections to all outputs which are listening for a given target.
   * @param silvverUid The silvver uid of target or camera being observed.
   * @param outputsConnections A vector of shared_prt with IoConnection to
   *                           all clients found.  */
  void findOutputs(KeyType silvverUid,
                   std::vector<boost::shared_ptr<IoConnection> >& outputsConnections);

private:
  friend class Singleton<OutputMultiMap<clientType, KeyType> >;

  typedef std::multimap<KeyType, boost::shared_ptr<IoConnection> > TMultiMap;

private:
  boost::shared_mutex accessMap;

  OutputMultiMap();

  TMultiMap outputs;
};

#endif /* _OUTPUT_MULTIMAP_HPP_ */
