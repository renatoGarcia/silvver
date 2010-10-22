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

#include "markerProcessor.hpp"

#include <boost/foreach.hpp>

#include "common/serializations.hpp"
#include "processor.ipp"

MarkerProcessor::MarkerProcessor(const procOpt::Marker& spec)
  :Processor<silvver::Pose>::Processor(procOpt::AnyProcOpt(spec))
  ,lastReadings()
  ,lastReadingsAccess()
{}

void
MarkerProcessor::deliverPackage(silvver::CameraReading<silvver::Pose>& reading)
{
  boost::mutex::scoped_lock lock(this->lastReadingsAccess);
  this->lastReadings[reading.uid] = reading;

  this->process(reading.localizations);
}

void
MarkerProcessor::process(std::vector<silvver::Identity<silvver::Pose> >& currentPoses) const
{
  std::vector<silvver::Identity<silvver::Pose> > allPoses;

  BOOST_FOREACH(const TMap::value_type& reading, this->lastReadings)
  {
    allPoses.insert(allPoses.end(),  reading.second.localizations.begin(),
                    reading.second.localizations.end());
  }

  // Merge if there are poses with same uid reported by diferent cameras
  std::vector<silvver::Identity<silvver::Pose> >::iterator itCurrent, itAll;
  for (itCurrent = currentPoses.begin();
       itCurrent < currentPoses.end();
       ++itCurrent)
  {
    for(itAll = allPoses.begin(); itAll < allPoses.end(); ++itAll)
    {
      if(itCurrent->uid == itAll->uid)
      {
        /// TODO: Make merge
        // itCurrent->x = (itFirst->x + itSecond->x) / 2;
        // itFirst->y = (itFirst->y + itSecond->y) / 2;
      }
    }
  }

  this->sendToOutputs(currentPoses);
}
