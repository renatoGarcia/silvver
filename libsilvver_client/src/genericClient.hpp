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

#ifndef _GENERIC_CLIENT_HPP_
#define _GENERIC_CLIENT_HPP_

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

template<class Spec>
class GenericClient
{
public:
  connection::Channel* constructChannel(const std::string& serverName,
                                        const std::string& receptionistPort);

  GenericClient(const typename Spec::UidType& uid,
                const std::string& serverName,
                const std::string& receptionistPort);

  ~GenericClient();

  // Callback method called when a new localization arrives.
  void handleReceive(connection::error_code ec);

  typename Spec::UidType getUid();

  typename Spec::LocalizationType getLast();

  typename Spec::LocalizationType getUnseen(const boost::posix_time::time_duration& waitTime);

  typename Spec::LocalizationType getNext(const boost::posix_time::time_duration& waitTime);

  const typename Spec::UidType uid;

  // Synchronize the access to localization attribute.
  boost::mutex mutexLocalization;

  // Condition of there to be an unseen localization.
  boost::condition_variable unseenLocalization;

  // Last received localization.
  typename Spec::LocalizationType localization;

  // Signalize a never seen localization.
  bool localizationIsUnseen;

  // Will holds a localization until copy it safely to
  // localization attribute.
  typename Spec::LocalizationType tmpLocalization;

  boost::asio::io_service ioService;

  boost::asio::io_service::work work;

  boost::thread serviceThread;

  boost::scoped_ptr<connection::Channel> serverChannel;
};

template<class Spec>
connection::Channel*
GenericClient<Spec>::constructChannel(const std::string& serverName,
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

template<class Spec>
GenericClient<Spec>::GenericClient(const typename Spec::UidType& uid,
                                   const std::string& serverName,
                                   const std::string& receptionistPort)
  :uid(uid)
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
  Request request = typename Spec::RequestType(this->uid);
  this->serverChannel->send(request);

  this->serverChannel->asyncReceive(this->tmpLocalization,
                                    boost::bind(&GenericClient<Spec>::handleReceive,
                                                this, _1));
}

template<class Spec>
GenericClient<Spec>::~GenericClient()
{}

template<class Spec>
void
GenericClient<Spec>::handleReceive(connection::error_code ec)
{
  if (ec == connection::error_code::broken_connection)
  {
    return;
  }

  if (this->tmpLocalization.uid == this->uid)
  {
    {
      boost::mutex::scoped_lock lock(this->mutexLocalization);
      this->localization = this->tmpLocalization;

      this->localizationIsUnseen = true;
    }

    this->unseenLocalization.notify_one();
  }

  this->serverChannel->asyncReceive(this->tmpLocalization,
                                    boost::bind(&GenericClient<Spec>::handleReceive,
                                                this, _1));
}

template<class Spec>
typename Spec::UidType
GenericClient<Spec>::getUid()
{
  return this->uid;
}

template<class Spec>
typename Spec::LocalizationType
GenericClient<Spec>::getLast()
{
  if (!this->serverChannel->isOpen())
  {
    throw silvver::connection_error("The silvver-server has closed the connection");
  }

  boost::mutex::scoped_lock lock(this->mutexLocalization);

  this->localizationIsUnseen = false;
  return this->localization;
}

template<class Spec>
typename Spec::LocalizationType
GenericClient<Spec>::getUnseen(const boost::posix_time::time_duration& waitTime)
{
  if (!this->serverChannel->isOpen())
  {
    throw silvver::connection_error("The silvver-server has closed the connection");
  }

  boost::mutex::scoped_lock lock(this->mutexLocalization);

  while (!this->localizationIsUnseen)
  {
    if (!this->unseenLocalization.timed_wait(lock, waitTime))
    {
      throw silvver::time_expired_error("The wait time expired without a new "
                                        "pose arrives");
    }
  }

  this->localizationIsUnseen = false;
  return this->localization;
}

template<class Spec>
typename Spec::LocalizationType
GenericClient<Spec>::getNext(const boost::posix_time::time_duration& waitTime)
{
  if (!this->serverChannel->isOpen())
  {
    throw silvver::connection_error("The silvver-server has closed the connection");
  }

  boost::mutex::scoped_lock lock(this->mutexLocalization);

  // Wait for handleReceive method notifies a new localization.
  if (!this->unseenLocalization.timed_wait(lock, waitTime))
  {
    throw silvver::time_expired_error("The wait time expired without the next "
                                      "pose arrives");
  }

  this->localizationIsUnseen = false;
  return this->localization;
}

#endif /* _GENERIC_CLIENT_HPP_ */
