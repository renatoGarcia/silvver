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

#ifndef _TARGET_HPP_
#define _TARGET_HPP_

#include <memory>
#include <stdexcept>
#include <string>

#include <boost/date_time/time_duration.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "silvverTypes.hpp"

namespace silvver
{

  class time_expired_error : public std::runtime_error
  {
  public:
    time_expired_error(const std::string& whatArg)
      :runtime_error(whatArg){}
  };

  template<class U> class CheshireCat;

  /// Represents a target looked by Silvver.
  template<class T>
  class Target
  {
  public:

    /** Target class constructor.
     * @param targetId Target identifier.
     * @param serverIp IP address of silvver-server.
     * @param receptionistPort Port number of silvver-server receptionist.
     */
    Target(unsigned targetId,
           const std::string& serverIp="127.0.0.1",
           unsigned receptionistPort=12000);

    ~Target() throw();

    /// Connect to the silvver-server.
    /// Can throw boost::system::system_error when silvver-server
    /// is unreachable.
    void connect();

    /// Can throw boost::system::system_error when silvver-server
    /// is unreachable.
    void disconnect();

    /** Get the id of target.
     * @return The id of this target.
     */
    unsigned getId();

    /// Get the last received target localization.
    Identity<T> getLast();

    /** Get a never gotten taget localization, or throw an
     * old_pose_error exception.
     * @param waitTime
     *
     * @return
     */
    Identity<T> getNew(const boost::posix_time::time_duration&
                       waitTime = boost::date_time::pos_infin);

    /** Return the next received target localization.
     * This function wait until a new Pose arrives and returns;
     */
    Identity<T> getNext(const boost::posix_time::time_duration&
                        waitTime = boost::date_time::pos_infin);

  private:

    std::auto_ptr<CheshireCat<T> > smile;
  };
}
#endif /* _TARGET_HPP_ */
