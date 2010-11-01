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

#include "target.hpp"

#include "genericClient.hpp"
#include "request.hpp"

namespace silvver {

template<class T>
class Target<T>::CheshireCat
  :public GenericClient<TargetUid, Identity<T>, AddTargetClient>
{
public:
  CheshireCat(const TargetUid& uid,
              const std::string& serverName,
              const std::string& receptionistPort)
    :GenericClient<TargetUid,
                   Identity<T>,
                   AddTargetClient>(uid, serverName, receptionistPort)
  {}
};

template<class T>
Target<T>::Target(const TargetUid& targetUid,
                  const std::string& serverName,
                  const std::string& receptionistPort)
  :smile(new CheshireCat(targetUid, serverName, receptionistPort))
{}

template<class T>
Target<T>::~Target() throw()
{}

template<class T>
TargetUid
Target<T>::getUid() const
{
  return smile->getUid();
}

template<class T>
Identity<T>
Target<T>::getLast()
{
  return smile->getLast();
}

template<class T>
Identity<T>
Target<T>::getUnseen(const boost::posix_time::time_duration& waitTime)
{
  return smile->getUnseen(waitTime);
}

template<class T>
Identity<T>
Target<T>::getNext(const boost::posix_time::time_duration& waitTime)
{
  return smile->getNext(waitTime);
}

// Templates to be compiled in library
template class Target<Position>;
template class Target<Pose>;

} // End namespace silvver
