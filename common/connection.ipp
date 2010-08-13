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

#ifndef _CONNECTION_IPP_
#define _CONNECTION_IPP_

#include "connection.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "serializations.hpp"

template <class T>
void
Connection::send(const T& t)
{
  std::string data;
  serialize(data, t);
  this->channel->send(data);
}

template <class T>
void
Connection::receive(T& t)
{
  std::string data;
  this->channel->receive(data);
  deserialize(t, data);
}

template <class T>
void
Connection::serialize(std::string& data, const T& t)
{
  std::ostringstream archiveStream;
  boost::archive::text_oarchive archive(archiveStream);
  archive << t;
  data = archiveStream.str();
}

template <class T>
void
Connection::deserialize(T& t, const std::string& data)
{
  try
  {
    std::istringstream archiveStream(data);
    boost::archive::text_iarchive archive(archiveStream);
    archive >> t;
  }
  catch (std::exception& e)
  {
    // Unable to decode data.
    throw data_error("Error deserializing data.");
  }
}

#endif /* _CONNECTION_IPP_ */
