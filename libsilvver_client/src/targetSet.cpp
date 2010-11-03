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

#include "targetSet.hpp"

#include "genericClient.hpp"
#include "request.hpp"

namespace silvver {

template<class T>
class TargetSet<T>::CheshireCat
  :public GenericClient<TargetSetUid, std::vector<Identity<T> >, AddTargetSetClient>
{
public:
  CheshireCat(const TargetSetUid& uid,
              const std::string& serverName,
              const std::string& receptionistPort)
    :GenericClient<TargetSetUid,
                   std::vector<Identity<T> >,
                   AddTargetSetClient        >(uid, serverName, receptionistPort)
  {}
};

template<class T>
TargetSet<T>::TargetSet(const TargetSetUid& targetSetUid,
                        const std::string& serverName,
                        const std::string& receptionistPort)
  :smile(new CheshireCat(targetSetUid, serverName, receptionistPort))
{}

template<class T>
TargetSet<T>::~TargetSet() throw()
{}

template<class T>
TargetSetUid
TargetSet<T>::getUid() const
{
  return smile->getUid();
}

template<class T>
std::vector<Identity<T> >
TargetSet<T>::getLast()
{
  return smile->getLast();
}

template<class T>
std::vector<Identity<T> >
TargetSet<T>::getUnseen(const boost::posix_time::time_duration& waitTime)
{
  return smile->getUnseen(waitTime);
}

template<class T>
std::vector<Identity<T> >
TargetSet<T>::getNext(const boost::posix_time::time_duration& waitTime)
{
  return smile->getNext(waitTime);
}

template<class T>
void
TargetSet<T>::exitWait()
{
  smile->exitWait();
}

// Templates to be compiled in library
template class TargetSet<Position>;
template class TargetSet<Pose>;

} // End namespace silvver
