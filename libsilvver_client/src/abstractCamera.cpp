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

#include <boost/bind.hpp>

#include "connection.ipp"

namespace silvver
{
  template<class T>
  class
  AbstractCamera<T>::CheshireCat
  {
  public:
    CheshireCat(boost::function<void(CameraReading<T>)> callback,
                const silvver::AbstractCameraUid& abstractCameraUid,
                const std::string& serverName,
                const std::string& receptionistPort);

    ~CheshireCat();

    // Callback method called when a new localization arrives.
    void handleReceive();

    boost::function<void (CameraReading<T>)> callbackFunction;

    const silvver::AbstractCameraUid abstractCameraUid;

    // Value of last received reading.
    CameraReading<T> currentReading;

    Connection connection;
  };

  template<class T> AbstractCamera<T>::
  CheshireCat::CheshireCat(boost::function<void(CameraReading<T>)> callback,
                           const silvver::AbstractCameraUid& abstractCameraUid,
                           const std::string& serverName,
                           const std::string& receptionistPort)
    :callbackFunction(callback)
    ,abstractCameraUid(abstractCameraUid)
    ,connection(serverName, receptionistPort, abstractCameraUid)
  {
    this->connection.asyncRead(this->currentReading,
                               boost::bind(&CheshireCat::handleReceive,
                                           this));
  }

  template<class T>
  AbstractCamera<T>::CheshireCat::~CheshireCat()
  {}

  template<class T>
  void
  AbstractCamera<T>::CheshireCat::handleReceive()
  {
    if (this->currentReading.camUid == this->abstractCameraUid)
    {
      this->callbackFunction(this->currentReading);
    }

    this->connection.asyncRead(this->currentReading,
                               boost::bind(&CheshireCat::handleReceive,
                                           this));
  }

  template<class T>
  silvver::AbstractCameraUid
  AbstractCamera<T>::getUid()
  {
    return smile->abstractCameraUid;
  }

  template<class T>
  AbstractCamera<T>::AbstractCamera(boost::function<void(CameraReading<T>)> callback,
                                    const silvver::AbstractCameraUid& abstractCameraUid,
                                    const std::string& serverName,
                                    const std::string& receptionistPort)
    :smile(new CheshireCat(callback, abstractCameraUid,
                           serverName, receptionistPort))
  {}

  template<class T>
  AbstractCamera<T>::~AbstractCamera() throw()
  {}

  // Templates to be compiled in library
  template class AbstractCamera<Position>;
  template class AbstractCamera<Pose>;

} // Namespace silvver
