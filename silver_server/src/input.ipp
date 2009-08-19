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

#include "processorFactory_impl.hpp"
#include <iostream>
#include <vector>
#include <boost/bind.hpp>
#include "silverTypes.hpp"

template <typename Type>
Input<Type>::Input(boost::shared_ptr<IoConnection> connection,
                   ProcessorType processorType)
  :InputInterface()
  ,connection(connection)
  ,connectionPort(connection->getLocalPort())
  ,processor(ProcessorFactory<Type>::createProcessor(processorType))
{
  this->connection->asyncReceive(this->inputs,
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
  this->processor->deliverPackage(this->inputs, this->connectionPort);

  this->connection->asyncReceive(this->inputs,
                                 boost::bind(&Input<Type>::handleReceive,
                                             this));
}

#endif /* _INPUT_IPP_ */
