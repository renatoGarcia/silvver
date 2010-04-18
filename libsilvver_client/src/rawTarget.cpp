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

#include "rawTarget.hpp"

#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>

#include "connection.ipp"

namespace silvver
{
  template<class U>
  class
  CheshireCat
  {
  public:
    CheshireCat(boost::function<void (Identity<U>)> callbackFunction,
                unsigned targetId,
                const std::string& serverIp,
                unsigned receptionistPort);

    ~CheshireCat();

    /// Callback method called when a new localization arrives.
    void handleReceive();

    boost::function<void (Identity<U>)> callbackFunction;

    const unsigned targetId;

    /// Value of last received target.
    Identity<U> currentTarget;

    boost::scoped_ptr<Connection> connection;

    bool connected;
  };

  template<class U>
  CheshireCat<U>::CheshireCat(boost::function<void (Identity<U>)> callbackFunction,
                              unsigned targetId,
                              const std::string& serverIp,
                              unsigned receptionistPort)
    :callbackFunction(callbackFunction)
    ,targetId(targetId)
    ,connection(new Connection(serverIp, receptionistPort))
    ,connected(false)
  {}

  template<class U>
  void
  CheshireCat<U>::handleReceive()
  {
    if (this->currentTarget.uid == this->targetId)
    {
      this->callbackFunction(currentTarget);
    }

    this->connection->asyncRead(this->currentTarget,
                                boost::bind(&CheshireCat<U>::handleReceive,
                                            this));
  }

  template<class T>
  void
  RawTarget<T>::connect()
  {
    if (!smile->connected)
    {
      smile->connection->connect(CLIENT_RAW, smile->targetId);
      smile->connected = true;

      smile->connection->asyncRead(smile->currentTarget,
                                   boost::bind(&CheshireCat<T>::handleReceive,
                                               smile.get()));
    }
  }

  template<class T>
  void
  RawTarget<T>::disconnect()
  {
    if (smile->connected)
    {
      smile->connection->disconnect(CLIENT_RAW, smile->targetId);
      smile->connected = false;
    }
  }

  template<class T>
  unsigned
  RawTarget<T>::getId()
  {
    return smile->targetId;
  }

  template<class T>
  RawTarget<T>::RawTarget(boost::function<void (Identity<T>)> callbackFunction,
                          unsigned targetId,
                          const std::string& serverIp,
                          unsigned receptionistPort)
    :smile(new CheshireCat<T>(callbackFunction, targetId,
                              serverIp, receptionistPort))
  {}

  template<class T>
  RawTarget<T>::~RawTarget() throw()
  {
    try
    {
      this->disconnect();
    }
    catch(...)
    {
      abort();
    }
  }

  // Templates to be compiled in library
  template class RawTarget<Position>;
  template class RawTarget<Pose>;

} //Namespace silvver
