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

#include "clientsMap.hpp"

#include "ioConnection.ipp"

ClientsMap::ClientsMap()
{}

void
ClientsMap::addOutput(unsigned targetId,
                      boost::shared_ptr<IoConnection> outputConnection)
{
  boost::mutex::scoped_lock lock(this->accessMap);

  this->client.insert(std::pair< unsigned, boost::shared_ptr<IoConnection> >
                      (targetId, outputConnection));
}

void
ClientsMap::delOutput(unsigned idTarget, unsigned remotePort)
{
  boost::mutex::scoped_lock lock(this->accessMap);

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

std::vector< boost::shared_ptr<IoConnection> >
ClientsMap::findClient(unsigned idTarget)
{
  boost::mutex::scoped_lock lock(this->accessMap);

  std::pair< TMultiMap::iterator, TMultiMap::iterator >
    range(this->client.equal_range(idTarget));

  std::vector< boost::shared_ptr<IoConnection> > vecConnection;
  for(TMultiMap::iterator it = range.first;
      it != range.second;
      ++it)
  {
    vecConnection.push_back(it->second);
  }

  return vecConnection;
}
