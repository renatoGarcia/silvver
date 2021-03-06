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
     * @param targetId Target silvver uid.
     * @param serverIp IP address of silvver-server.
     * @param receptionistPort Port number of silvver-server receptionist. */
    Target(const TargetUid& targetUid,
           const std::string& serverIp="127.0.0.1",
           unsigned receptionistPort=12000);

    ~Target() throw();

    /** Connect to the silvver-server.
     * Can throw connection_error when silvver-server is unreachable. */
    void connect();

    /// Can throw connection_error when silvver-server is unreachable.
    void disconnect();

    /** Get the UID of target.
     * @return The UID of this target.  */
    TargetUid getUid();

    /// Get the last received target localization.
    Identity<T> getLast();

    /** Get a never gotten taget localization, or throw an
     * old_pose_error exception.
     * @param waitTime
     *
     * @return  */
    Identity<T> getNew(const boost::posix_time::time_duration&
                       waitTime = boost::date_time::pos_infin);

    /** Return the next received target localization.
     * This function wait until a new Pose arrives and returns */
    Identity<T> getNext(const boost::posix_time::time_duration&
                        waitTime = boost::date_time::pos_infin);

  private:
    class CheshireCat;
    std::auto_ptr<CheshireCat> smile;
  };
}
#endif /* _SILVVER_TARGET_HPP_ */
