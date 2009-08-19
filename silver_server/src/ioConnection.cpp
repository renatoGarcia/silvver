/* Copyright 2009 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#include "ioConnection.ipp"

boost::asio::io_service IoConnection::ioService;
boost::scoped_ptr<boost::thread> IoConnection::th;
boost::once_flag IoConnection::onceFlag = BOOST_ONCE_INIT;
boost::asio::io_service::work IoConnection::work(IoConnection::ioService);

void
IoConnection::runIoService()
{
  try
  {
    IoConnection::ioService.run();
  }
  // This system_error is threw when
  catch(boost::system::system_error& e)
  {}
}

IoConnection::IoConnection(const std::string& remoteIpAddress,
                           const unsigned short remotePort)
  :socket(IoConnection::ioService, boost::asio::ip::udp::endpoint())
  ,inboundData(UPD_MAX_LENGTH)
{
  boost::asio::ip::udp::endpoint
    ep(boost::asio::ip::address::from_string(remoteIpAddress), remotePort);
  this->socket.connect(ep);

  boost::call_once(IoConnection::onceFlag,
                   boost::bind(&boost::scoped_ptr<boost::thread>::reset,
                               &IoConnection::th,
                               new boost::thread(IoConnection::runIoService)));
}

IoConnection::~IoConnection()
{
  if(this->socket.is_open())
  {
    try
    {
      this->socket.shutdown(boost::asio::ip::udp::socket::shutdown_both);
      this->socket.close();
    }
    catch(...)
    {
      std::cerr << "Error on closing the UDP socket" << std::endl;
    }
  }
}
