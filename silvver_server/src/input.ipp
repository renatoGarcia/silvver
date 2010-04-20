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

#ifndef _INPUT_IPP_
#define _INPUT_IPP_

#include "input.hpp"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

template <typename Type>
Input<Type>::Input(boost::shared_ptr<IoConnection> connection,
                   boost::shared_ptr< ProcessorInterface<Type> > processor)
  :InputInterface()
  ,currentInput()
  ,connection(connection)
  ,connectionPort(connection->getLocalPort())
  ,processor(processor)
  ,clientCameraMap(OutputMultiMap<CLIENT_CAMERA, std::string>::instantiate())
{

  this->connection->asyncReceive(this->currentInput,
                                 boost::bind(&Input<Type>::handleReceive,
                                             this));
}

template <typename Type>
Input<Type>::~Input()
{}

template <typename Type>
void
Input<Type>::handleReceive()
{
  std::vector<boost::shared_ptr<IoConnection> > vecConnections;
  boost::shared_ptr<IoConnection> connectionPtr;

  // Get all camera clients hearing for a given camera.
  this->clientCameraMap->findOutputs(this->currentInput.abstractCameraUid,
                                     vecConnections);
  BOOST_FOREACH(connectionPtr, vecConnections)
  {
    connectionPtr->send(this->currentInput);
  }

  this->processor->deliverPackage(this->currentInput);

  this->connection->asyncReceive(this->currentInput,
                                 boost::bind(&Input<Type>::handleReceive,
                                             this));
}

#endif /* _INPUT_IPP_ */
