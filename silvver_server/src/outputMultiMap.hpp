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

#include "connection.hpp"
#include "singleton.hpp"

/** A class to hold the connected outputs.
 * This class wrap the STL multimap for thread safety. */
template<class KeyType>
class OutputMultiMap
  :public Singleton<OutputMultiMap<KeyType> >
{
public:
  /** Add a output.
   * @param silvverUid The silvver uid of target or camera being observed.
   * @param connection A Connection already made with the output.  */
  void addOutput(KeyType silvverUid, boost::shared_ptr<Connection> connection);

  /** Delete a output.
   * @param silvverUid The silvver uid of target or camera being observed.
   * @param connection A pointer to connection of the output to be deleted. */
  void delOutput(KeyType silvverUid, boost::shared_ptr<Connection> connection);

  /** Return connections to all outputs which are listening for a given target.
   * @param silvverUid The silvver uid of target or camera being observed.
   * @param outputsConnections A vector of shared_prt with Connections to
   *                           all clients found.  */
  void findOutputs(KeyType silvverUid,
                   std::vector<boost::shared_ptr<Connection> >& outputsConnections);

private:
  friend class Singleton<OutputMultiMap<KeyType> >;

  typedef std::multimap<KeyType, boost::shared_ptr<Connection> > TMultiMap;

private:
  OutputMultiMap();

  boost::shared_mutex accessMap;

  TMultiMap outputs;
};

#endif /* _OUTPUT_MULTIMAP_HPP_ */
