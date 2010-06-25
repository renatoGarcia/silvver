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
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include "connection.ipp"

namespace silvver
{
  template<class T>
  class
  AbstractCamera<T>::CheshireCat
  {
  public:
    CheshireCat(const silvver::AbstractCameraUid& abstractCameraUid,
                const std::string& serverName,
                const std::string& receptionistPort);

    ~CheshireCat();

    // Callback method called when a new cameraReading arrives.
    void handleReceive();

    const silvver::AbstractCameraUid abstractCameraUid;

    // Outside callback function called when a new cameraReading arrives.
    boost::function<void (CameraReading<T>)> callback;

    // Synchronize the access to cameraReading attribute.
    boost::mutex mutexCameraReading;

    // Condition of there to be an unseen cameraReading.
    boost::condition_variable unseenCameraReading;

    // Last received cameraReading.
    CameraReading<T> cameraReading;

    // Signalize a never seen cameraReading.
    bool cameraReadingIsUnseen;

    // Will holds a cameraReading until copy it safely to
    // cameraReading attribute.
    CameraReading<T> tmpCameraReading;

    Connection serverConnection;
  };

  template<class T> AbstractCamera<T>::
  CheshireCat::CheshireCat(const silvver::AbstractCameraUid& abstractCameraUid,
                           const std::string& serverName,
                           const std::string& receptionistPort)
    :abstractCameraUid(abstractCameraUid)
    ,callback()
    ,mutexCameraReading()
    ,unseenCameraReading()
    ,cameraReading()
    ,cameraReadingIsUnseen(false)
    ,tmpCameraReading()
    ,serverConnection(serverName, receptionistPort, abstractCameraUid)
  {
    this->serverConnection.asyncRead(this->cameraReading,
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
    if (this->tmpCameraReading.camUid == this->abstractCameraUid)
    {
      {
        boost::mutex::scoped_lock lock(this->mutexCameraReading);
        this->cameraReading = this->tmpCameraReading;

        this->cameraReadingIsUnseen = true;

        if (this->callback)
        {
          this->callback(this->cameraReading);
          this->cameraReadingIsUnseen = false;
        }
      }

      this->unseenCameraReading.notify_one();
    }

    this->serverConnection.asyncRead(this->tmpCameraReading,
                                     boost::bind(&CheshireCat::handleReceive,
                                                 this));
  }

  template<class T>
  void
  AbstractCamera<T>::setCallback(boost::function<void (CameraReading<T>)> callback)
  {
    smile->callback = callback;
  }

  template<class T>
  silvver::AbstractCameraUid
  AbstractCamera<T>::getUid()
  {
    return smile->abstractCameraUid;
  }

  template<class T>
  CameraReading<T>
  AbstractCamera<T>::getLast()
  {
    if (!smile->serverConnection.isOpen())
    {
      throw connection_error("The silvver-server has closed the connection");
    }

    boost::mutex::scoped_lock lock(smile->mutexCameraReading);

    smile->cameraReadingIsUnseen = false;
    return smile->cameraReading;
  }

  template<class T>
  CameraReading<T>
  AbstractCamera<T>::getUnseen(const boost::posix_time::time_duration& waitTime)
  {
    if (!smile->serverConnection.isOpen())
    {
      throw connection_error("The silvver-server has closed the connection");
    }

    boost::mutex::scoped_lock lock(smile->mutexCameraReading);

    while (!smile->cameraReadingIsUnseen)
    {
      if (!smile->unseenCameraReading.timed_wait(lock, waitTime))
      {
        throw typename
          silvver::time_expired_error("The wait time expired without a new "
                                      "pose arrives");
      }
    }

    smile->cameraReadingIsUnseen = false;
    return smile->cameraReading;
  }

  template<class T>
  CameraReading<T>
  AbstractCamera<T>::getNext(const boost::posix_time::time_duration& waitTime)
  {
    if (!smile->serverConnection.isOpen())
    {
      throw connection_error("The silvver-server has closed the connection");
    }

    boost::mutex::scoped_lock lock(smile->mutexCameraReading);

    // Wait for handleReceive method notifies a new cameraReading.
    if (!smile->unseenCameraReading.timed_wait(lock, waitTime))
    {
      throw typename
        silvver::time_expired_error("The wait time expired without the next "
                                    "pose arrives");
    }

    smile->cameraReadingIsUnseen = false;
    return smile->cameraReading;
  }

  template<class T>
  AbstractCamera<T>::AbstractCamera(const silvver::AbstractCameraUid& abstractCameraUid,
                                    const std::string& serverName,
                                    const std::string& receptionistPort)
    :smile(new CheshireCat(abstractCameraUid, serverName, receptionistPort))
  {}

  template<class T>
  AbstractCamera<T>::~AbstractCamera() throw()
  {}

  // Templates to be compiled in library
  template class AbstractCamera<Position>;
  template class AbstractCamera<Pose>;

} // Namespace silvver
