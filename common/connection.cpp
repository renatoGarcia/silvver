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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "connection.hpp"
#include "connection.ipp"

#include <memory>

#include "tcpIp.hpp"
#include "ipc.hpp"

Connection::Connection()
  :channel()
{}

Connection::~Connection()
{}

void
Connection::close()
{
  this->channel->close();
}

void
Connection::waitConnection(const TcpIpEp& receptionistEp)
{
  std::string tmpData;
  Endpoint serverEp;
  ChannelsEnum channelType;

  std::auto_ptr<TcpIp> receptionist(new TcpIp(receptionistEp));
  receptionist->accept();
  receptionist->receive(tmpData);
  deserialize(channelType, tmpData);

  if (channelType == TCP_IP)
  {
    this->channel.reset(receptionist.release());
  }
  else if (channelType == IPC)
  {
    this->channel.reset(new Ipc());
    serialize(tmpData, this->channel->getEndpoint());
    receptionist->send(tmpData);
  }
  else
  {
    throw connection_error("Unknown channel type");
  }
}

void
Connection::connect(const TcpIpEp& receptionistEp, ChannelsEnum channelType)
{
  if (channelType == TCP_IP)
  {
    std::string tmpData;
    TcpIpEp autoLocalEp;

    TcpIp* receptionist = new TcpIp(autoLocalEp, receptionistEp);
    serialize(tmpData, channelType);
    receptionist->send(tmpData);

    this->channel.reset(receptionist);
  }
  else if (channelType == IPC)
  {
    std::string tmpData;
    Endpoint ep;

    TcpIp* receptionist = new TcpIp(TcpIpEp(), receptionistEp);
    serialize(tmpData, channelType);
    receptionist->send(tmpData);

    receptionist->receive(tmpData);
    deserialize(ep, tmpData);

    this->channel.reset(new Ipc(boost::get<IpcEp>(ep)));
  }
  else
  {
    throw connection_error("Unknown channel type");
  }
}
