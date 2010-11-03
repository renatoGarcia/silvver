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

#include "clientMultiMap.hpp"

#include "common/connection/channel.ipp"
#include "common/silvverTypes.hpp"

template<class KeyType>
ClientMultiMap<KeyType>::ClientMultiMap()
{}

template<class KeyType>
void
ClientMultiMap<KeyType>::addClient(KeyType uid, ChannelPointer channel)
{
  boost::unique_lock<boost::shared_mutex> lock(this->accessMap);

  this->clients.insert(std::make_pair(uid, channel));
}

template<class KeyType>
void
ClientMultiMap<KeyType>::delClient(KeyType uid, ChannelPointer channel)
{
  boost::unique_lock<boost::shared_mutex> lock(this->accessMap);

  // Get all clients whit the idTarget
  std::pair<typename TMultiMap::iterator, typename TMultiMap::iterator>
    range(this->clients.equal_range(uid));

  // Look for client connected at remote port remotePort and delete it
  for(typename TMultiMap::iterator it = range.first;
      it != range.second;
      ++it)
  {
    if(it->second == channel)
    {
      this->clients.erase(it);
    }
  }
}

template<class KeyType>
void
ClientMultiMap<KeyType>::findClients(KeyType uid,
                                     std::vector<ChannelPointer>& clientChannels)
{
  clientChannels.clear();

  boost::shared_lock<boost::shared_mutex> lock(this->accessMap);

  std::pair<typename TMultiMap::iterator, typename TMultiMap::iterator>
    range(this->clients.equal_range(uid));

  for(typename TMultiMap::iterator it = range.first;
      it != range.second;
      ++it)
  {
    clientChannels.push_back(it->second);
  }
}

template class ClientMultiMap<silvver::TargetSetUid>;
template class ClientMultiMap<silvver::TargetUid>;
template class ClientMultiMap<silvver::AbstractCameraUid>;
