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

#include "ipc.hpp"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/lexical_cast.hpp>

namespace bip = boost::interprocess;
namespace bpt = boost::posix_time;

int Ipc::uidCounter = 0;

std::string
Ipc::genMQName()
{
  std::string name = "silvver_mq_" + boost::lexical_cast<std::string>(Ipc::uidCounter);
  Ipc::uidCounter++;

  return name;
}

Ipc::Ipc()
  :Channel()
  ,inBuffer()
  ,messageQueue()
  ,mqName(Ipc::genMQName())
  ,hbThread()
  ,connectionIsBroken(false)
{
  bip::message_queue::remove(this->mqName.c_str());
  this->messageQueue.reset(new bip::message_queue(bip::create_only,
                                                  this->mqName.c_str(),
                                                  1, 1024));

  this->hbThread.reset(new boost::thread(&Ipc::heartBeatServer, this));
}

Ipc::Ipc(const IpcEp& ipcEp)
  :Channel()
  ,inBuffer()
  ,messageQueue(new bip::message_queue(bip::open_only, ipcEp.ep.c_str()))
  ,mqName(ipcEp.ep)
  ,hbThread()
  ,connectionIsBroken(false)
{
  this->hbThread.reset(new boost::thread(&Ipc::heartBeatClient, this));
}

Ipc::~Ipc() throw()
{
  try
  {
    this->close();
  }
  catch(...)
  {}

  bip::message_queue::remove(this->mqName.c_str());
}

void
Ipc::close()
{
  if (this->hbThread)
  {
    this->hbThread->interrupt();
    this->hbThread->join();
  }
}

Endpoint
Ipc::getEndpoint() const
{
  return IpcEp(this->mqName);
}

void
Ipc::send(const std::string& data)
{
  try
  {
    while (!this->messageQueue->timed_send(data.data(), data.size(), 0,
                                           bpt::second_clock::local_time() +
                                           bpt::seconds(5)))
    {
      if (this->connectionIsBroken)
        throw broken_connection("Lost connection");
    }
  }
  catch(const bip::interprocess_exception& e)
  {
    throw data_error(e.what());
  }
}

void
Ipc::receive(std::string& data)
{
  std::size_t received;
  unsigned int priority;

  try
  {
    while (!this->messageQueue->timed_receive(this->inBuffer.c_array(),
                                              this->inBuffer.size(),
                                              received,
                                              priority,
                                              bpt::second_clock::local_time() +
                                              bpt::seconds(5)))
    {
      if (this->connectionIsBroken)
        throw broken_connection("Lost connection");
    }
  }
  catch(const bip::interprocess_exception& e)
  {
    throw data_error(e.what());
  }

  data.assign(this->inBuffer.data(), received);
}

void
Ipc::heartBeatServer()
{
  const std::string heartBeatName(this->mqName + "_hb");
  const char message = '.';
  const std::size_t size = sizeof(message);

  bip::message_queue::remove(heartBeatName.c_str());
  bip::message_queue heartBeatMQ(bip::create_only, heartBeatName.c_str(),1, 8);

  while (true)
  {
    if (!heartBeatMQ.timed_send(&message, sizeof(message), 0,
                                bpt::second_clock::local_time() +
                                bpt::seconds(5)))
    {
      // A mutex is not needed, because the concurrent access in it don't is
      // critical
      this->connectionIsBroken = true;

      return;
    }
    boost::this_thread::sleep(boost::posix_time::seconds(1));
  }
}

void
Ipc::heartBeatClient()
{
  const std::string heartBeatName(this->mqName + "_hb");
  bip::message_queue heartBeatMQ(bip::open_only, heartBeatName.c_str());

  std::size_t rcvSize;
  const std::size_t bufferSize = 10;
  char buffer[bufferSize];
  unsigned priority;
  while (true)
  {
    if (!heartBeatMQ.timed_receive(&buffer, bufferSize, rcvSize, priority,
                                   bpt::second_clock::local_time() +
                                   bpt::seconds(5)))
    {
      // A mutex is not needed, because the concurrent access in it don't is
      // critical
      this->connectionIsBroken = true;

      return;
    }
    boost::this_thread::sleep(boost::posix_time::seconds(1));
  }
}
