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

#ifndef _PROCESSOR_IPP_
#define _PROCESSOR_IPP_

#include "processor.hpp"

#include <boost/foreach.hpp>

#include "ioConnection.ipp"
#include "debugOutputs.hpp"
#include <silverTypes.hpp>

template <class Tinput, class Toutput>
Processor<Tinput,Toutput>::Processor()
{
  this->clientsMap = ClientsMap::instantiate();
}

template <class Tinput, class Toutput>
void
Processor<Tinput,Toutput>::sendToOutputs(const std::vector<Toutput> &localizations) const
{
  std::vector< boost::shared_ptr<IoConnection> > vecConnection;
  boost::shared_ptr<IoConnection> connectionPtr;

  BOOST_FOREACH(Toutput output, localizations)
  {
    debug::logOut << ts_output::lock
                  << output <<  std::endl
                  << ts_output::unlock;

    // Get all clients hearing for a given target.
    vecConnection = this->clientsMap->findClient(output.uid);

    BOOST_FOREACH(connectionPtr, vecConnection)
    {
      connectionPtr->send(output);
    }
  }

}

#endif /*  _PROCESSOR_IPP_ */
