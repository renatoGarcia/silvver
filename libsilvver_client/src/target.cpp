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

#include "target.hpp"

#include <boost/bind.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <cstdlib>

#include "connection.ipp"

namespace silvver
{
  template<class T>
  class
  Target<T>::CheshireCat
  {
  public:
    CheshireCat(const silvver::TargetUid& targetUid,
                const std::string& serverName,
                const std::string& receptionistPort);

    ~CheshireCat();

    // Callback method called when a new localization arrives.
    void handleReceive();

    const silvver::TargetUid targetUid;

    // Outside callback function called when a new localization arrives.
    boost::function<void (Identity<T>)> callback;

    // Synchronize the access to localization attribute.
    boost::mutex mutexLocalization;

    // Condition of there to be an unseen localization.
    boost::condition_variable unseenLocalization;

    // Last received localization.
    Identity<T> localization;

    // Signalize if current localization was never returned/read.
    bool localizationIsUnseen;

    // Will holds a localization until copy it safely to
    // localization attribute.
    Identity<T> tmpLocalization;

    Connection serverConnection;
  };

  template<class T>
  Target<T>::CheshireCat::CheshireCat(const silvver::TargetUid& targetUid,
                                      const std::string& serverName,
                                      const std::string& receptionistPort)
    :targetUid(targetUid)
    ,callback()
    ,mutexLocalization()
    ,unseenLocalization()
    ,localization()
    ,localizationIsUnseen(false)
    ,tmpLocalization()
    ,serverConnection(serverName, receptionistPort, targetUid)
  {
    this->serverConnection.asyncRead(this->tmpLocalization,
                                     boost::bind(&CheshireCat::handleReceive,
                                                 this));
  }

  template<class T>
  Target<T>::CheshireCat::~CheshireCat()
  {}

  template<class T>
  void
  Target<T>::CheshireCat::handleReceive()
  {
    if (this->tmpLocalization.uid == this->targetUid)
    {
      {
        boost::mutex::scoped_lock lock(this->mutexLocalization);
        this->localization = this->tmpLocalization;

        this->localizationIsUnseen = true;

        if (this->callback)
        {
          this->callback(this->localization);
          this->localizationIsUnseen = false;
        }
      }

      this->unseenLocalization.notify_one();
    }

    this->serverConnection.asyncRead(this->tmpLocalization,
                                     boost::bind(&CheshireCat::handleReceive,
                                                 this));
  }

  template<class T>
  void
  Target<T>::setCallback(boost::function<void (Identity<T>)> callback)
  {
    smile->callback = callback;
  }

  template<class T>
  silvver::TargetUid
  Target<T>::getUid()
  {
    return smile->targetUid;
  }

  template<class T>
  Identity<T>
  Target<T>::getLast()
  {
    if (!smile->serverConnection.isOpen())
    {
      throw connection_error("The silvver-server has closed the connection");
    }

    boost::mutex::scoped_lock lock(smile->mutexLocalization);

    smile->localizationIsUnseen = false;
    return smile->localization;
  }

  template<class T>
  Identity<T>
  Target<T>::getUnseen(const boost::posix_time::time_duration& waitTime)
  {
    if (!smile->serverConnection.isOpen())
    {
      throw connection_error("The silvver-server has closed the connection");
    }

    boost::mutex::scoped_lock lock(smile->mutexLocalization);

    while (!smile->localizationIsUnseen)
    {
      if (!smile->unseenLocalization.timed_wait(lock, waitTime))
      {
        throw typename
          silvver::time_expired_error("The wait time expired without a new "
                                      "pose arrives");
      }
    }

    smile->localizationIsUnseen = false;
    return smile->localization;
  }

  template<class T>
  Identity<T>
  Target<T>::getNext(const boost::posix_time::time_duration& waitTime)
  {
    if (!smile->serverConnection.isOpen())
    {
      throw connection_error("The silvver-server has closed the connection");
    }

    boost::mutex::scoped_lock lock(smile->mutexLocalization);

    // Wait for handleReceive method notifies a new pose.
    if (!smile->unseenLocalization.timed_wait(lock, waitTime))
    {
      throw typename
        silvver::time_expired_error("The wait time expired without the next "
                                    "pose arrives");
    }

    smile->localizationIsUnseen = false;
    return smile->localization;
  }

  template<class T>
  Target<T>::Target(const silvver::TargetUid& targetUid,
                    const std::string& serverName,
                    const std::string& receptionistPort)
    :smile(new CheshireCat(targetUid, serverName, receptionistPort))
  {}

  template<class T>
  Target<T>::~Target() throw()
  {}

  // Templates to be compiled in library
  template class Target<Position>;
  template class Target<Pose>;

} // End namespace silvver
