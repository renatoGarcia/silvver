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

namespace bpt = boost::posix_time;

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

    /// Callback method called when a new localization arrives.
    void update();

    const silvver::TargetUid targetUid;

    /// Synchronize the write and read in current.
    boost::mutex mutexCurrentPose;

    boost::condition_variable newPoseCondition;

    /// Value of last received Pose.
    Identity<T> current;

    // Signalize a never returned/read current.
    bool currentIsNew;

    /// Will holds an Ente until convert it safely to
    /// current locking mutexCurrentPose.
    Identity<T> last;

    Connection connection;
  };

  template<class T>
  Target<T>::CheshireCat::CheshireCat(const silvver::TargetUid& targetUid,
                                      const std::string& serverName,
                                      const std::string& receptionistPort)
    :targetUid(targetUid)
    ,currentIsNew(false)
    ,connection(serverName, receptionistPort, targetUid)
  {
    this->connection.asyncRead(this->last,
                               boost::bind(&CheshireCat::update,
                                           this));
  }

  template<class T>
  Target<T>::CheshireCat::~CheshireCat()
  {}

  template<class T>
  void
  Target<T>::CheshireCat::update()
  {
    if (this->last.uid == this->targetUid)
    {
      {
        boost::mutex::scoped_lock lock(this->mutexCurrentPose);
        this->current = this->last;

        this->currentIsNew = true;
      }

      this->newPoseCondition.notify_one();
    }

    this->connection.asyncRead(this->last,
                               boost::bind(&CheshireCat::update,
                                           this));
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
    boost::mutex::scoped_lock lock(smile->mutexCurrentPose);

    smile->currentIsNew = false;
    return smile->current;
  }

  template<class T>
  Identity<T>
  Target<T>::getNew(const boost::posix_time::time_duration& waitTime)
  {
    boost::mutex::scoped_lock lock(smile->mutexCurrentPose);

    while (!smile->currentIsNew)
    {
      if (!smile->newPoseCondition.timed_wait(lock,
                                              bpt::second_clock::universal_time() +
                                              waitTime))
      {
        throw typename
          silvver::time_expired_error("The wait time expired without a new "
                                      "pose arrives");
      }
    }

    smile->currentIsNew = false;
    return smile->current;
  }

  template<class T>
  Identity<T>
  Target<T>::getNext(const boost::posix_time::time_duration& waitTime)
  {
    boost::mutex::scoped_lock lock(smile->mutexCurrentPose);

    // Wait for update function notifies a new pose.
    if (!smile->newPoseCondition.timed_wait(lock,
                                            bpt::second_clock::universal_time() +
                                            waitTime))
    {
      throw typename
        silvver::time_expired_error("The wait time expired without the next "
                                    "pose arrives");
    }

    smile->currentIsNew = false;
    return smile->current;
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
