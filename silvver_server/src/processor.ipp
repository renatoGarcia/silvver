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

#ifndef _PROCESSOR_IPP_
#define _PROCESSOR_IPP_

#include "processor.hpp"

#include <boost/foreach.hpp>

#include "common/connection/channel.ipp"
#include "log.hpp"

template<class Tinput>
Processor<Tinput>::Processor(const procOpt::AnyProcOpt& spec)
  :ProcessorBase()
  ,outputMap(OutputMultiMap<silvver::TargetUid>::instantiate())
  ,processorSpec(spec)
{}

template<class Tinput>
bool
Processor<Tinput>::isSameSpec(const procOpt::AnyProcOpt& spec)
{
  return this->processorSpec == spec;
}

template <class Tinput>
template <class Toutput>
void
Processor<Tinput>::sendToOutputs
         (const std::vector<silvver::Identity<Toutput> >& localizations) const
{
  std::vector<ChannelPointer> vecChannels;
  ChannelPointer channelPtr;

  BOOST_FOREACH(silvver::Identity<Toutput> output, localizations)
  {
    targetsLog(TargetsLogLevel::INFO)
      << ts_output::lock
      << output <<  std::endl
      << ts_output::unlock;

    // Get all clients hearing for a given target.
    this->outputMap->findOutputs(output.uid, vecChannels);

    BOOST_FOREACH(channelPtr, vecChannels)
    {
      channelPtr->send(output);
    }
  }
}

#endif /*  _PROCESSOR_IPP_ */
