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

#include "outputMultiMap.hpp"

#include "ioConnection.ipp"

template<ClientType clientType, class KeyType>
OutputMultiMap<clientType, KeyType>::
OutputMultiMap()
{}

template<ClientType clientType, class KeyType>
void OutputMultiMap<clientType, KeyType>::
addOutput(KeyType silvverUid,
          boost::shared_ptr<IoConnection> outputConnection)
{
  boost::unique_lock<boost::shared_mutex> lock(this->accessMap);

  this->outputs.insert(std::make_pair(silvverUid, outputConnection));
}

template<ClientType clientType, class KeyType>
void OutputMultiMap<clientType, KeyType>::
delOutput(KeyType silvverUid, unsigned remotePort)
{
  boost::unique_lock<boost::shared_mutex> lock(this->accessMap);

  // Get all clients whit the idTarget
  std::pair<typename TMultiMap::iterator, typename  TMultiMap::iterator>
    range(this->outputs.equal_range(silvverUid));

  // Look for client connected at remote port remotePort and delete it
  for(typename TMultiMap::iterator it = range.first;
      it != range.second;
      ++it)
  {
    if(it->second->getRemotePort() == remotePort)
    {
      this->outputs.erase(it);
    }
  }
}

template<ClientType clientType, class KeyType>
void OutputMultiMap<clientType, KeyType>::
findOutputs(KeyType silvverUid,
            std::vector<boost::shared_ptr<IoConnection> >& outputsConnections)
{
  outputsConnections.clear();

  boost::shared_lock<boost::shared_mutex> lock(this->accessMap);

  std::pair<typename TMultiMap::iterator, typename TMultiMap::iterator>
    range(this->outputs.equal_range(silvverUid));

  for(typename TMultiMap::iterator it = range.first;
      it != range.second;
      ++it)
  {
    outputsConnections.push_back(it->second);
  }
}

template class OutputMultiMap<CLIENT_TARGET, unsigned>;
template class OutputMultiMap<CLIENT_CAMERA, std::string>;
