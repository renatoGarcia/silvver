/* Copyright 2009-2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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
#include <boost/variant/static_visitor.hpp>

#include "common/connection/channel.hpp"
#include "common/processorOptions.hpp"
#include "common/silvverTypes.hpp"
#include "inputInterface.hpp"

class InputFactory
{
public:
  static
  InputInterface* create(const silvver::TargetSetUid& targetSetUid,
                         const procOpt::AnyProcOpt& processorOpt,
                         boost::shared_ptr<connection::Channel> channel);

private:
  class Visitor
    :public boost::static_visitor<InputInterface*>
  {
  public:
    Visitor(const silvver::TargetSetUid& targetSetUid,
            boost::shared_ptr<connection::Channel> channel);

    template <class T>
    InputInterface* operator()(const T& processorSpec) const;

  private:
    const silvver::TargetSetUid targetSetUid;

    const boost::shared_ptr<connection::Channel> channel;
  };

  /** Private constructor not implemented.
   * This class cannot be instantiated. */
  InputFactory();
};

#endif /* _INPUT_FACTORY_HPP_ */
