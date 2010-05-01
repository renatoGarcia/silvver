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

#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <string>

#include "processorOptions.hpp"
#include "scene.hpp"
#include "silvverTypes.hpp"

class Connection
{
public:

  Connection(const std::string& serverIp, unsigned receptionistPort,
             const silvver::AbstractCameraUid& abstractCameraUid);

  ~Connection();

  void connect(const procOpt::AnyProcOpt& processorOpt);

  void disconnect();

  template <typename T>
  void send(const T& t);

private:

  friend class boost::thread;

  static void runIoService();

  static const unsigned HEADER_LENGTH = 8;

  static boost::asio::io_service ioService;

  static boost::scoped_ptr<boost::thread> th;

  static boost::once_flag onceFlag;

  const silvver::AbstractCameraUid abstractCameraUid;

  boost::asio::ip::tcp::socket receptionistSocket;

  const boost::asio::ip::tcp::endpoint receptionistEP;

  boost::asio::ip::udp::socket outputSocket;

  template <typename T>
  void writeToReceptionist(const T& t);

  template <typename T>
  void readFromReceptionist(T& t);
};

#endif // _CONNECTION_HPP_
