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

#ifndef _CHANNEL_HPP_
#define _CHANNEL_HPP_

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <string>

#include "channelTypes.hpp"
#include "exceptions.hpp"

class Channel
{
public:
  typedef boost::function<void (connection::error_code)> ReceiveHandler;

  virtual ~Channel()
  {}

  /** Connect to a peer.
   *
   * @param acceptorEp The endpoint of acceptor on peer side.
   */
  template <class AcceptorType>
  void connect(const typename AcceptorType::Endpoint& acceptorEp);

  /** Synchronously send a value of type T.
   * This method will block until t has been sent.
   *
   * @param t The value to be sent.
   */
  template <class T>
  void send(const T& t);

  /** Synchronously receive a value of type T.
   * This method will block until t has been received.
   *
   * @param t Where put the received value.
   */
  template <class T>
  void receive(T& t);

  /** Asynchronously receive a value of type T.
   *
   * @param t Reference to the variable where put the received value.
   * @param handler Functional with signature
   *        void handler(connection::error_code ec), which will be
   *        called when the operation completes.
   */
  template <class T>
  void asyncReceive(T& t, ReceiveHandler handler);

  /// Close the current connection.
  virtual void close()=0;

  /// Return the local endpoint.
  virtual AnyEndpoint localEndpoint() const=0;

  void
  setCloseHandler(const boost::function<void (void)>& closeHandler)
  {
    this->closeHandler = closeHandler;
  }

protected:
  typedef boost::function<void (const boost::system::error_code& ec,
                                std::size_t bytes_transferred       )>
  ReadHandler;

  Channel(ChannelType channelType)
    :channelType(channelType)
  {}

private:

  /** Block until data be sent.
   * This method can throw broken_connection and data_error.
   *
   * @param data A string with the data to be sent.  */
  virtual void write(const std::string& data)=0;

  /** Block until receive a data package.
   * This method can throw broken_connection and data_error.
   *
   * @param data A string where put the received data.  */
  virtual void read(std::string& data)=0;

  virtual void asyncRead(boost::shared_ptr<std::string> data,
                         ReadHandler handler)=0;

  virtual void effectConnection(const std::string& strAcceptorEp)=0;

  template <class T>
  void readHandler(boost::shared_ptr<std::string> data,
                   T& t,
                   ReceiveHandler handler,
                   const boost::system::error_code& ec,
                   std::size_t bytes_transferred);

  const ChannelType channelType;

  boost::function<void (void)> closeHandler;
};

#endif /* _CHANNEL_HPP_ */
