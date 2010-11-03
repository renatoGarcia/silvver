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

#ifndef _CLIENT_MULTIMAP_HPP_
#define _CLIENT_MULTIMAP_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <map>
#include <vector>

#include "common/connection/channel.hpp"
#include "common/singleton.hpp"

/** A class to hold the connected outputs.
 * This class wrap the STL multimap for thread safety. */
template<class KeyType>
class ClientMultiMap
  :public Singleton<ClientMultiMap<KeyType> >
{
public:
  typedef boost::shared_ptr<connection::Channel> ChannelPointer;

  /** Adds a client.
   * @param uid The uid of connected client.
   * @param channel A Channel already connected with the client.  */
  void addClient(KeyType uid, ChannelPointer channel);

  /** Deletes a client.
   * @param uid The uid of client to be removed.
   * @param channel A pointer to channel of the client to be removed. */
  void delClient(KeyType uid, ChannelPointer channel);

  /** Returns channels to all clients which are listening for a given target.
   * @param uid The uid of client.
   * @param clientChannels A vector of shared_prt with Channels to
            all clients found.
  */
  void findClients(KeyType uid, std::vector<ChannelPointer>& clientChannels);

private:
  friend class Singleton<ClientMultiMap<KeyType> >;

  typedef std::multimap<KeyType, ChannelPointer> TMultiMap;

  ClientMultiMap();

  boost::shared_mutex accessMap;

  TMultiMap clients;
};

#endif /* _CLIENT_MULTIMAP_HPP_ */
