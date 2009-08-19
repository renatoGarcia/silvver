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

#ifndef _INPUT_HPP_
#define _INPUT_HPP_

#include "inputInterface.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "ioConnection.hpp"
#include "processorInterface.hpp"
#include "processorFactory.hpp"

template <typename Type>
class Input : public InputInterface
{
public:

  Input(boost::shared_ptr<IoConnection> connection,
        ProcessorType processorType);

  ~Input();

private:

  std::vector<Type> inputs;

  boost::shared_ptr<IoConnection> connection;

  unsigned connectionPort;

  boost::shared_ptr< ProcessorInterface<Type> > processor;

  void handleReceive();
};

#endif /* _INPUT_HPP_ */
