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
                const std::string& abstractCameraUid,
                const std::string& serverIp,
                unsigned receptionistPort);

    ~CheshireCat();

    /// Callback method called when a new localization arrives.
    void handleReceive();

    boost::function<void (CameraReading<T>)> callbackFunction;

    const std::string abstractCameraUid;

    /// Value of last received reading.
    CameraReading<T> currentReading;

    Connection connection;

    bool connected;
  };

  template<class T> AbstractCamera<T>::
  CheshireCat::CheshireCat(boost::function<void(CameraReading<T>)> callback,
                           const std::string& abstractCameraUid,
                           const std::string& serverIp,
                           unsigned receptionistPort)
    :callbackFunction(callback)
    ,abstractCameraUid(abstractCameraUid)
    ,connection(serverIp, receptionistPort)
    ,connected(false)
  {}

  template<class T>
  AbstractCamera<T>::CheshireCat::~CheshireCat()
  {}

  template<class T>
  void
  AbstractCamera<T>::CheshireCat::handleReceive()
  {
    if (this->currentReading.abstractCameraUid == this->abstractCameraUid)
    {
      this->callbackFunction(this->currentReading);
    }

    this->connection.asyncRead(this->currentReading,
                               boost::bind(&CheshireCat::handleReceive,
                                           this));
  }

  // template<class T>
  // void
  // AbstractCamera<T>::setCallback(boost::function<void(CameraReading<T>)> function)
  // {
  //   smile->callbackFunction = function;
  // }

  template<class T>
  void
  AbstractCamera<T>::connect()
  {
    if (!smile->connected)
    {
      try
      {
        smile->connection.connect(smile->abstractCameraUid);
        smile->connected = true;

        smile->connection.asyncRead(smile->currentReading,
                                    boost::bind(&CheshireCat::handleReceive,
                                                smile.get()));
      }
      catch (const boost::system::system_error& e)
      {
        throw silvver::connection_error(e.what());
      }
    }
  }

  template<class T>
  void
  AbstractCamera<T>::disconnect()
  {
    if (smile->connected)
    {
      try
      {
        smile->connection.disconnect(smile->abstractCameraUid);
        smile->connected = false;
      }
      catch (const boost::system::system_error& e)
      {
        throw silvver::connection_error(e.what());
      }
    }
  }

  template<class T>
  std::string
  AbstractCamera<T>::getId()
  {
    return smile->abstractCameraUid;
  }

  template<class T>
  AbstractCamera<T>::AbstractCamera(boost::function<void(CameraReading<T>)> callback,
                                    const std::string& abstractCameraUid,
                                    const std::string& serverIp,
                                    unsigned receptionistPort)
    :smile(new CheshireCat(callback, abstractCameraUid,
                           serverIp, receptionistPort))
  {}

  template<class T>
  AbstractCamera<T>::~AbstractCamera() throw()
  {
    try
    {
      this->disconnect();
    }
    catch(...)
    {}
  }

  // Templates to be compiled in library
  template class AbstractCamera<Position>;
  template class AbstractCamera<Pose>;

} //Namespace silvver
