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

#include "inputFactory.hpp"

#include <boost/variant/apply_visitor.hpp>

#include "common/connection/channel.ipp"
#include "common/silvverTypes.hpp"
#include "input.ipp"
#include "markerProcessor.hpp"
#include "processorFactory.hpp"

InputInterface*
InputFactory::create(const silvver::TargetSetUid& targetSetUid,
                     const procOpt::AnyProcOpt& processorOpt,
                     boost::shared_ptr<connection::Channel> channel)
{
  return boost::apply_visitor(Visitor(targetSetUid, channel), processorOpt);
}

InputFactory::Visitor::Visitor(const silvver::TargetSetUid& targetSetUid,
                               boost::shared_ptr<connection::Channel> channel)
  :targetSetUid(targetSetUid)
  ,channel(channel)
{}

template <class T>
InputInterface*
InputFactory::Visitor::operator()(const T& processorSpec) const
{
  return new Input<typename T::InputType>
                              (this->channel,
                               ProcessorFactory::create(this->targetSetUid,
                                                        processorSpec));
}
