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

#ifndef _SILVVER_TARGET_SET_HPP_
#define _SILVVER_TARGET_SET_HPP_

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/time_duration.hpp>
#include <memory>
#include <string>
#include <vector>

#include "silvverTypes.hpp"
#include "exceptions.hpp"

namespace silvver {

/// Represents a targetSet observed by Silvver.
template<class T>
class TargetSet
{
public:
  /** TargetSet client class constructor.
   * Can throw silvver::connection_error
   *
   * @param targetSetUid TargetSetUid to be observed.
   * @param serverName IP address or hostname of silvver-server.
   * @param receptionistPort Port number of silvver-server receptionist.
   */
  TargetSet(const TargetSetUid& targetSetUid,
            const std::string& serverName="localhost",
            const std::string& receptionistPort="12000");

  ~TargetSet() throw();

  /** Get the targetSetUid of observed targetSet.
   *
   * @return The targetSetUid.
   */
  TargetSetUid getUid() const;

  /** Get the last received localizations of observed targetSet.
   * Immediately return the last received localizations, even if it was
   * already seen. This method can throw silvver::connection_error if the
   * connection with silvver-server is closed.
   *
   * @return A std::vector with the last received localizations.
   */
  std::vector<Identity<T> > getLast();

  /** Get a never seen localizations of observed targetSet.
   * This method will wait for waitTime until throw a
   * silvver::time_expired_error exception. If the connection with
   * silvver-server is closed, it will throw a silvver::connection_error.
   *
   * @param waitTime The time to wait for before throw the exception. The
   *        default waitTime is infinity.
   * @return A std::vector with the localizations.
   */
  std::vector<Identity<T> > getUnseen(const boost::posix_time::time_duration&
                                      waitTime = boost::date_time::pos_infin);

  /** Get the next localizations of observed targetSet.
   * This method will wait until a new set of localizations arrives from
   * silvver-server. If that don't arrives in waitTime a
   * silvver::time_expired_error exception will be threw. If the connection
   * with silvver-server is closed, it will throw a silvver::connection_error.
   *
   * @param waitTime The time to wait for before throw the exception. The
   *        default waitTime is infinity.
   * @return A std::vector with the localizations.
   */
  std::vector<Identity<T> > getNext(const boost::posix_time::time_duration&
                                    waitTime = boost::date_time::pos_infin);

private:
  class CheshireCat;
  std::auto_ptr<CheshireCat> smile;
};

} // namespace silvver

#endif /* _SILVVER_TARGET_SET_HPP_ */
