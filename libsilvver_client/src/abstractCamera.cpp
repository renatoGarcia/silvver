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

#include "abstractCamera.hpp"

#include "genericClient.hpp"
#include "request.hpp"

namespace silvver {

template<class T>
class AbstractCamera<T>::CheshireCat
  :public GenericClient<AbstractCameraUid, CameraReading<T>, AddCameraClient>
{
public:
  CheshireCat(const AbstractCameraUid& uid,
              const std::string& serverName,
              const std::string& receptionistPort)
    :GenericClient<AbstractCameraUid,
                   CameraReading<T>,
                   AddCameraClient   >(uid, serverName, receptionistPort)
  {}
};

template<class T>
AbstractCamera<T>::AbstractCamera(const AbstractCameraUid& abstractCameraUid,
                                  const std::string& serverName,
                                  const std::string& receptionistPort)
  :smile(new CheshireCat(abstractCameraUid, serverName, receptionistPort))
{}

template<class T>
AbstractCamera<T>::~AbstractCamera() throw()
{}

template<class T>
AbstractCameraUid
AbstractCamera<T>::getUid()
{
  return smile->getUid();
}

template<class T>
CameraReading<T>
AbstractCamera<T>::getLast()
{
  return smile->getLast();
}

template<class T>
CameraReading<T>
AbstractCamera<T>::getUnseen(const boost::posix_time::time_duration& waitTime)
{
  return smile->getUnseen(waitTime);
}

template<class T>
CameraReading<T>
AbstractCamera<T>::getNext(const boost::posix_time::time_duration& waitTime)
{
  return smile->getNext(waitTime);
}

// Templates to be compiled in library
template class AbstractCamera<Position>;
template class AbstractCamera<Pose>;

} // namespace silvver
