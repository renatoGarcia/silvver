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

#ifndef _SILVVER_TARGET_HPP_
#define _SILVVER_TARGET_HPP_

#include <memory>
#include <stdexcept>
#include <string>

#include <boost/date_time/time_duration.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "exceptions.hpp"
#include "silvverTypes.hpp"

namespace silvver
{
  /// Represents a target looked by Silvver.
  template<class T>
  class Target
  {
  public:
    /** Target class constructor.
     * Can throw silvver::connection_error
     * @param targetUid Target silvver uid.
     * @param serverName IP address or hostname of silvver-server.
     * @param receptionistPort Port number of silvver-server receptionist. */
    Target(const TargetUid& targetUid,
           const std::string& serverName="localhost",
           const std::string& receptionistPort="12000");

    ~Target() throw();

    /** Get the UID of target.
     * @return The UID of this target.  */
    TargetUid getUid();

    /** Get the last received target localization.
     * Immediately return the last received target, even if it was already
     * read. This method can throw silvver::connection_error if the connection
     * with silvver-server is closed.
     * @return The last received target localization. */
    Identity<T> getLast();

    /** Get a never gotten taget localization.
     * This method will wait for waitTime until throw a
     * silvver::time_expired_error exception. If the connection with
     * silvver-server is closed, it will throw a silvver::connection_error.
     * @param waitTime The time to wait for before throw the exception. The
     *                 default waitTime is infinity.
     * @return The target localization. */
    Identity<T> getNew(const boost::posix_time::time_duration&
                       waitTime = boost::date_time::pos_infin);

    /** Get the next target localization.
     * This method will wait until a new target localization arrives from
     * silvver-server. If that don't arrives in waitTime a
     * silvver::time_expired_error exception will be threw. If the connection
     * with silvver-server is closed, it will throw a
     * silvver::connection_error.
     * @param waitTime The time to wait for before throw the exception. The
     *                 default waitTime is infinity.
     * @return The target localization. */
    Identity<T> getNext(const boost::posix_time::time_duration&
                        waitTime = boost::date_time::pos_infin);

  private:
    class CheshireCat;
    std::auto_ptr<CheshireCat> smile;
  };
}
#endif /* _SILVVER_TARGET_HPP_ */
