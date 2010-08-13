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

#ifndef _CHANNEL_HPP_
#define _CHANNEL_HPP_

#include <string>

#include "channelTypes.hpp"
#include "exceptions.hpp"

class Channel
{
public:
  virtual ~Channel()
  {}

  virtual void close()=0;

  virtual Endpoint getEndpoint() const=0;

  /** Block until data be sent.
   * This method can throw broken_connection and data_error.
   *
   * @param data A string with the data to be sent.  */
  virtual void send(const std::string& data)=0;

  /** Block until receive a data package.
   * This method can throw broken_connection and data_error.
   *
   * @param data A string where put the received data.  */
  virtual void receive(std::string& data)=0;

protected:
  Channel()
  {}
};

#endif /* _CHANNEL_HPP_ */
