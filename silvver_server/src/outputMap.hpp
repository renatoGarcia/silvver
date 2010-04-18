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

#ifndef _OUTPUT_MAP_HPP_
#define _OUTPUT_MAP_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <map>
#include <vector>

#include "ioConnection.hpp"
#include <singleton.hpp>

enum OutputType
{
  OUTPUT_NORMAL,
  OUTPUT_RAW
};

/** A class to hold the connected clients.
 * This class wrap the STL multimap for thread safety.
 */
template<OutputType outputType>
class OutputMap: public Singleton<OutputMap<outputType> >
{
public:

  /** Add a client.
   * @param targetId The silvver uid of target observed by client.
   * @param outputConnection An IoConnection already connected with the client.
   */
  void addOutput(unsigned targetId,
                 boost::shared_ptr<IoConnection> outputConnection);

  /** Delete a client
   * @param idTarget The id of target which the client is listening.
   * @param remotePort The remote port where the client is connected, given by
   *                   IoConnection::getRemotePort()
   */
  void delOutput(unsigned idTarget, unsigned remotePort);

  /** Return connections to all clients which are listening for a given target.
   * @param idTarget The id of desired target.
   * @param clientsConnections A vector of shared_prt with IoConnection to
                               all clients found.  */
  void findOutputs(unsigned idTarget,
                   std::vector<boost::shared_ptr<IoConnection> >& clientsConnections);

private:
  friend class Singleton<OutputMap<outputType> >;

  typedef std::multimap<unsigned, boost::shared_ptr<IoConnection> > TMultiMap;

private:
  boost::shared_mutex accessMap;

  /// A dummy private constructor.
  OutputMap();

  /// The key of multimap is the id of target
  TMultiMap client;
};

#endif /* _OUTPUT_MAP_HPP_ */
