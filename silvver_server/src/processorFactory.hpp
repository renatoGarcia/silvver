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

#ifndef _PROCESSOR_FACTORY_HPP_
#define _PROCESSOR_FACTORY_HPP_

#include <boost/shared_ptr.hpp>
#include <map>

#include "common/processorOptions.hpp"
#include "common/silvverTypes.hpp"
#include "processor.hpp"
#include "processorBase.hpp"

class ProcessorFactory
{
public:
  static
  Processor<silvver::Pose>*
  create(const silvver::TargetSetUid& targetSetUid,
         const procOpt::Marker& spec);

private:
  typedef
  std::map<silvver::TargetSetUid, boost::shared_ptr<ProcessorBase> >
  ProcessorMap;

  /// Map with all created Processors.
  static
  ProcessorMap createdProcessors;

  ProcessorFactory();
  virtual ~ProcessorFactory();
};

#endif /* _PROCESSOR_FACTORY_HPP_ */
