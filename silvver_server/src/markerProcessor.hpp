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

#ifndef _MARKER_PROCESSOR_HPP_
#define _MARKER_PROCESSOR_HPP_

#include <boost/thread/mutex.hpp>
#include <map>
#include <string>

#include "common/silvverTypes.hpp"
#include "common/singleton.hpp"
#include "processor.hpp"

class MarkerProcessor: public Processor<silvver::Pose, silvver::Pose>,
                       public Singleton<MarkerProcessor>

{
public:
  void deliverPackage(silvver::CameraReading<silvver::Pose>& reading);

private:
  /// Map with cameraUid as key, and its last reading as value.
  typedef std::map<silvver::AbstractCameraUid,
                   silvver::CameraReading<silvver::Pose> > TMap;

  friend class Singleton<MarkerProcessor>;

private:
  /// Hold the last reading of each camera.
  TMap lastReadings;

  MarkerProcessor();

  void process(std::vector<silvver::Identity<silvver::Pose> >& currentPoses) const;

  boost::mutex mutexArmazenador;
};

#endif /* _MARKER_PROCESSOR_HPP_ */
