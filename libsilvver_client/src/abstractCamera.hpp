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
     * @param abstractCameraUid AbstractCamera silvver uid.
     * @param serverIp IP address of silvver-server.
     * @param receptionistPort Port number of silvver-server receptionist. */
    AbstractCamera(boost::function<void (CameraReading<T>)> callback,
                   const AbstractCameraUid& abstractCameraUid,
                   const std::string& serverIp="127.0.0.1",
                   unsigned receptionistPort=12000);

    ~AbstractCamera() throw();

    /** Connect to the silvver-server.
     * Can throw connection_error when silvver-server is unreachable. */
    void connect();

    /// Can throw connection_error when silvver-server is unreachable.
    void disconnect();

    /** Get the id of camera.
     * @return The id of this camera.
     */
    AbstractCameraUid getUid();

  private:
    class CheshireCat;
    std::auto_ptr<CheshireCat> smile;
  };
}

#endif /* _SILVVER_ABSTRACT_CAMERA_HPP_ */
