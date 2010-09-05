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
#include <boost/scoped_ptr.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <cstdlib>

#include "connection/channel.ipp"
#include "connection/unixSocket.hpp"
#include "connection/tcpIp.hpp"
#include "request.hpp"
#include "serializations.hpp"

namespace silvver {

template<class T>
class
Target<T>::CheshireCat
{
public:
  connection::Channel* constructChannel(const std::string& serverName,
                                        const std::string& receptionistPort);

  CheshireCat(const silvver::TargetUid& targetUid,
              const std::string& serverName,
              const std::string& receptionistPort);

  ~CheshireCat();

  // Callback method called when a new target localization arrives.
  void handleReceive(connection::error_code ec);

  const silvver::TargetUid targetUid;

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

  boost::asio::io_service ioService;

  boost::asio::io_service::work work;

  boost::thread serviceThread;

  boost::scoped_ptr<connection::Channel> serverChannel;
};

template<class T>
connection::Channel*
Target<T>::CheshireCat::constructChannel(const std::string& serverName,
                                         const std::string& receptionistPort)
{
  using namespace connection;

  std::auto_ptr<Channel> channel;

  if ((serverName == "localhost") || (serverName == "127.0.0.1"))
  {
    channel.reset(new UnixSocket(this->ioService));
  }
  else
  {
    channel.reset(new TcpIp(this->ioService));
  }

  TcpIpEp serverEp = TcpIp::resolve(serverName, receptionistPort).front();
  channel->connect<TcpIp>(serverEp);

  return channel.release();
}

template<class T>
Target<T>::CheshireCat::CheshireCat(const silvver::TargetUid& targetUid,
                                    const std::string& serverName,
                                    const std::string& receptionistPort)
  :targetUid(targetUid)
  ,mutexLocalization()
  ,unseenLocalization()
  ,localization()
  ,localizationIsUnseen(false)
  ,tmpLocalization()
  ,ioService()
  ,work(ioService)
  ,serviceThread(static_cast<std::size_t (boost::asio::io_service::*)()>(&boost::asio::io_service::run),
                 &this->ioService)
  ,serverChannel(constructChannel(serverName, receptionistPort))
{
  Request request = AddTargetClient(this->targetUid);
  this->serverChannel->send(request);

  this->serverChannel->asyncReceive(this->tmpLocalization,
                                    boost::bind(&CheshireCat::handleReceive,
                                                this, _1));
}

template<class T>
Target<T>::CheshireCat::~CheshireCat()
{}

template<class T>
void
Target<T>::CheshireCat::handleReceive(connection::error_code ec)
{
  if (ec == connection::error_code::broken_connection)
  {
    return;
  }

  if (this->tmpLocalization.uid == this->targetUid)
  {
    {
      boost::mutex::scoped_lock lock(this->mutexLocalization);
      this->localization = this->tmpLocalization;

      this->localizationIsUnseen = true;
    }

    this->unseenLocalization.notify_one();
  }

  this->serverChannel->asyncReceive(this->tmpLocalization,
                                    boost::bind(&CheshireCat::handleReceive,
                                                this, _1));
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
  if (!smile->serverChannel->isOpen())
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
  if (!smile->serverChannel->isOpen())
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
  if (!smile->serverChannel->isOpen())
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
