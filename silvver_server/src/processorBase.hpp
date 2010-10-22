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

#ifndef _PROCESSOR_BASE_HPP_
#define _PROCESSOR_BASE_HPP_

#include "common/processorOptions.hpp"

/// Common base to all template classes Processor.
class ProcessorBase
{
public:
  // Check if spec is the same of this processor.
  virtual
  bool isSameSpec(const procOpt::AnyProcOpt& spec) = 0;
};

#endif /* _PROCESSOR_BASE_HPP_ */