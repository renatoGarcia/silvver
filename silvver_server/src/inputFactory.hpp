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

#ifndef _INPUT_FACTORY_HPP_
#define _INPUT_FACTORY_HPP_

#include <boost/shared_ptr.hpp>

#include "inputInterface.hpp"
#include "input.hpp"
#include "ioConnection.hpp"
#include <processorOptions.hpp>

class InputFactory
{
public:

  static boost::shared_ptr<InputInterface>
  createInput(const procOpt::AnyProcOpt& processorOpt,
              const boost::shared_ptr<IoConnection>& connection);

private:

  InputFactory();
};

#endif /* _INPUT_FACTORY_HPP_ */
