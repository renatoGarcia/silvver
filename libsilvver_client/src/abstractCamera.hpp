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

#ifndef _SILVVER_ABSTRACT_CAMERA_HPP_
#define _SILVVER_ABSTRACT_CAMERA_HPP_

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/time_duration.hpp>
#include <boost/function.hpp>
#include <memory>
#include <string>

#include "exceptions.hpp"
#include "silvverTypes.hpp"

namespace silvver
{
  template<class T>
  class AbstractCamera
  {
  public:
    /** AbstractCamera class constructor.
     * Can throw silvver::connection_error
     * @param abstractCameraUid  Silvver UID of abstractCamera to observe.
     * @param serverName IP address or hostname of silvver-server.
     * @param receptionistPort Port number of silvver-server receptionist. */
    AbstractCamera(const AbstractCameraUid& abstractCameraUid,
                   const std::string& serverName="localhost",
                   const std::string& receptionistPort="12000");

    ~AbstractCamera() throw();

    /** Set a callback function which will be called to all received
     * cameraReading.
     * A zero value will unset the current callback.
     * @param callback A function with signature
     *                 "void callback(cameraReading<T>)"  */
     void setCallback(boost::function<void (CameraReading<T>)> callback=0);

    /** Get the UID of observed abstractCamera.
     * @return The UID of abstractCamera. */
    AbstractCameraUid getUid();

    /** Get the last received cameraReading.
     * Immediately return the last received cameraReading, even if it was
     * already seen. This method can throw silvver::connection_error if the
     * connection with silvver-server is closed.
     * @return The last received cameraReading. */
    CameraReading<T> getLast();

    /** Get a never seen cameraReading.
     * This method will wait for waitTime until throw a
     * silvver::time_expired_error exception. If the connection with
     * silvver-server is closed, it will throw a silvver::connection_error.
     * @param waitTime The time to wait for before throw the exception. The
     *                 default waitTime is infinity.
     * @return The cameraReading. */
    CameraReading<T> getUnseen(const boost::posix_time::time_duration&
                               waitTime = boost::date_time::pos_infin);

    /** Get the next cameraReading.
     * This method will wait until a new cameraReading arrives from
     * silvver-server. If that don't arrives in waitTime a
     * silvver::time_expired_error exception will be threw. If the connection
     * with silvver-server is closed, it will throw a
     * silvver::connection_error.
     * @param waitTime The time to wait for before throw the exception. The
     *                 default waitTime is infinity.
     * @return The cameraReading. */
    CameraReading<T> getNext(const boost::posix_time::time_duration&
                             waitTime = boost::date_time::pos_infin);

  private:
    class CheshireCat;
    std::auto_ptr<CheshireCat> smile;
  };
}

#endif /* _SILVVER_ABSTRACT_CAMERA_HPP_ */
