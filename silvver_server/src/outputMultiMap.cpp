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

#include "connection.ipp"
#include "silvverTypes.hpp"

template<class KeyType>
OutputMultiMap<KeyType>::OutputMultiMap()
{}

template<class KeyType>
void
OutputMultiMap<KeyType>::addOutput(KeyType silvverUid,
                                   boost::shared_ptr<Connection> connection)
{
  boost::unique_lock<boost::shared_mutex> lock(this->accessMap);

  this->outputs.insert(std::make_pair(silvverUid, connection));
}

template<class KeyType>
void
OutputMultiMap<KeyType>::delOutput(KeyType silvverUid,
                                   boost::shared_ptr<Connection> connection)
{
  boost::unique_lock<boost::shared_mutex> lock(this->accessMap);

  // Get all clients whit the idTarget
  std::pair<typename TMultiMap::iterator, typename TMultiMap::iterator>
    range(this->outputs.equal_range(silvverUid));

  // Look for client connected at remote port remotePort and delete it
  for(typename TMultiMap::iterator it = range.first;
      it != range.second;
      ++it)
  {
    if(it->second == connection)
    {
      this->outputs.erase(it);
    }
  }
}

template<class KeyType>
void
OutputMultiMap<KeyType>::findOutputs(KeyType silvverUid,
                                     std::vector<boost::shared_ptr<Connection> >& outputsConnections)
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

template class OutputMultiMap<silvver::TargetUid>;
template class OutputMultiMap<silvver::AbstractCameraUid>;
