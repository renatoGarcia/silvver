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

#include "outputMap.hpp"

#include "ioConnection.ipp"

template<OutputType T>
OutputMap<T>::OutputMap()
{}

template<OutputType T>
void
OutputMap<T>::addOutput(unsigned targetId,
                         boost::shared_ptr<IoConnection> outputConnection)
{
  boost::unique_lock<boost::shared_mutex> lock(this->accessMap);

  this->client.insert(std::pair< unsigned, boost::shared_ptr<IoConnection> >
                      (targetId, outputConnection));
}

template<OutputType T>
void
OutputMap<T>::delOutput(unsigned idTarget, unsigned remotePort)
{
  boost::unique_lock<boost::shared_mutex> lock(this->accessMap);

  // Get all clients whit the idTarget
  std::pair< TMultiMap::iterator, TMultiMap::iterator >
    range(this->client.equal_range(idTarget));

  // Look for client connected at remote port remotePort and delete it
  for(TMultiMap::iterator it = range.first;
      it != range.second;
      ++it)
  {
    if(it->second->getRemotePort() == remotePort)
    {
      this->client.erase(it);
    }
  }
}

template<OutputType T>
void
OutputMap<T>::findOutputs(unsigned idTarget,
                          std::vector<boost::shared_ptr<IoConnection> >& clientsConnections)
{
  clientsConnections.clear();

  boost::shared_lock<boost::shared_mutex> lock(this->accessMap);

  std::pair< TMultiMap::iterator, TMultiMap::iterator >
    range(this->client.equal_range(idTarget));

  for(TMultiMap::iterator it = range.first;
      it != range.second;
      ++it)
  {
    clientsConnections.push_back(it->second);
  }
}

template class OutputMap<OUTPUT_NORMAL>;
template class OutputMap<OUTPUT_RAW>;
