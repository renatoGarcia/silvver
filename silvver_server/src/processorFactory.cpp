/* Copyright 2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#include "processorFactory.hpp"

#include "exceptions.hpp"
#include "markerProcessor.hpp"

ProcessorFactory::ProcessorMap ProcessorFactory::createdProcessors;

Processor<silvver::Pose>*
ProcessorFactory::create(const silvver::TargetSetUid& targetSetUid,
                         const procOpt::Marker& spec)
{
  ProcessorMap::iterator it = createdProcessors.find(targetSetUid);

  // If there aren't a processor to targetSetUid.
  if (it == createdProcessors.end())
  {
    boost::shared_ptr<ProcessorBase> processorPtr(new MarkerProcessor(spec,
                                                                      targetSetUid));
    it = createdProcessors.insert(std::make_pair(targetSetUid, processorPtr)).first;
  }

  // If the the processor specification is the same.
  if (it->second->isSameSpec(spec))
  {
    return dynamic_cast<Processor<silvver::Pose>*>(it->second.get());
  }
  else
  {
    throw conflicting_targetsets("Different processor specifications to a "
                                 "same targetSetUid");
  }
}
