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

#include "tcpIp.hpp"

#include "asioStream.ipp"

namespace bip = boost::asio::ip;

std::vector<bip::tcp::endpoint>
TcpIp::resolve(const std::string& address, const std::string& port)
{
  boost::asio::io_service ioService;

  std::vector<bip::tcp::endpoint> endpoints;
  bip::tcp::resolver resolver(ioService);
  bip::tcp::resolver::query query(address, port);

  bip::tcp::resolver::iterator endpoint_iterator, end;
  for (endpoint_iterator = resolver.resolve(query);
       endpoint_iterator != end;
       ++endpoint_iterator)
  {
    endpoints.push_back(*endpoint_iterator);
  }

  return endpoints;
}

TcpIp::TcpIp(boost::asio::io_service& ioService)
  :AsioStream<TcpIpSpec>(ioService)
{}

TcpIp::TcpIp(boost::asio::io_service& ioService, const TcpIpEp& localEp)
  :AsioStream<TcpIpSpec>(ioService, localEp)
{}

TcpIp::~TcpIp() throw()
{}
