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
AbstractCamera<T>::CheshireCat
{
public:
  connection::Channel* constructChannel(const std::string& serverName,
                                        const std::string& receptionistPort);

  CheshireCat(const silvver::AbstractCameraUid& abstractCameraUid,
              const std::string& serverName,
              const std::string& receptionistPort);

  ~CheshireCat();

  // Callback method called when a new cameraReading arrives.
  void handleReceive(connection::error_code ec);

  const silvver::AbstractCameraUid abstractCameraUid;

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

  boost::asio::io_service ioService;

  boost::asio::io_service::work work;

  boost::thread serviceThread;

  boost::scoped_ptr<connection::Channel> serverChannel;
};

template<class T>
connection::Channel*
AbstractCamera<T>::CheshireCat::constructChannel(const std::string& serverName,
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

template<class T> AbstractCamera<T>::
CheshireCat::CheshireCat(const silvver::AbstractCameraUid& abstractCameraUid,
                         const std::string& serverName,
                         const std::string& receptionistPort)
  :abstractCameraUid(abstractCameraUid)
  ,mutexCameraReading()
  ,unseenCameraReading()
  ,cameraReading()
  ,cameraReadingIsUnseen(false)
  ,tmpCameraReading()
  ,ioService()
  ,work(ioService)
  ,serviceThread(static_cast<std::size_t (boost::asio::io_service::*)()>(&boost::asio::io_service::run),
                 &this->ioService)
  ,serverChannel(constructChannel(serverName, receptionistPort))
{
  Request request = AddCameraClient(this->abstractCameraUid);
  this->serverChannel->send(request);

  this->serverChannel->asyncReceive(this->cameraReading,
                                    boost::bind(&CheshireCat::handleReceive,
                                                this, _1));
}

template<class T>
AbstractCamera<T>::CheshireCat::~CheshireCat()
{}

template<class T>
void
AbstractCamera<T>::CheshireCat::handleReceive(connection::error_code ec)
{
  if (ec == connection::error_code::broken_connection)
  {
    return;
  }

  if (this->tmpCameraReading.camUid == this->abstractCameraUid)
  {
    {
      boost::mutex::scoped_lock lock(this->mutexCameraReading);
      this->cameraReading = this->tmpCameraReading;

      this->cameraReadingIsUnseen = true;
    }

    this->unseenCameraReading.notify_one();
  }

  this->serverChannel->asyncReceive(this->tmpCameraReading,
                                    boost::bind(&CheshireCat::handleReceive,
                                                this, _1));
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
  if (!smile->serverChannel->isOpen())
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
  if (!smile->serverChannel->isOpen())
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
  if (!smile->serverChannel->isOpen())
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

} // namespace silvver
