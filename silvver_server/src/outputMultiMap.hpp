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

#ifndef _OUTPUT_MULTIMAP_HPP_
#define _OUTPUT_MULTIMAP_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <map>
#include <vector>

#include "common/connection/channel.hpp"
#include "common/singleton.hpp"

/** A class to hold the connected outputs.
 * This class wrap the STL multimap for thread safety. */
template<class KeyType>
class OutputMultiMap
  :public Singleton<OutputMultiMap<KeyType> >
{
public:
  typedef boost::shared_ptr<connection::Channel> ChannelPointer;

  /** Add a output.
   * @param silvverUid The silvver uid of target or camera being observed.
   * @param channel A Channel already made with the output.  */
  void addOutput(KeyType silvverUid, ChannelPointer channel);

  /** Delete a output.
   * @param silvverUid The silvver uid of target or camera being observed.
   * @param channel A pointer to channel of the output to be deleted. */
  void delOutput(KeyType silvverUid, ChannelPointer channel);

  /** Return channels to all outputs which are listening for a given target.
   * @param silvverUid The silvver uid of target or camera being observed.
   * @param outputsChannels A vector of shared_prt with Channels to
   *                           all clients found.  */
  void findOutputs(KeyType silvverUid,
                   std::vector<ChannelPointer>& outputsChannels);

private:
  friend class Singleton<OutputMultiMap<KeyType> >;

  typedef std::multimap<KeyType, ChannelPointer> TMultiMap;

  OutputMultiMap();

  boost::shared_mutex accessMap;

  TMultiMap outputs;
};

#endif /* _OUTPUT_MULTIMAP_HPP_ */
