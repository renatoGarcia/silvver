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

#ifndef _PROCESSOR_HPP_
#define _PROCESSOR_HPP_

#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>

#include "outputMap.hpp"
#include "processorInterface.hpp"

/// Abstract base class to the concrete processor classes.
template <class Tinput, class Toutput>
class Processor : public ProcessorInterface<Tinput>
{
protected:
  Processor();

  /** Send the final localizations to clients hearing for it.
   *
   * @param localizations A vector with all target localized.
   */
  void sendToOutputs(const std::vector<Toutput> &localizations) const;

private:
  boost::shared_ptr<OutputMap<OUTPUT_NORMAL> > outputMap;
};

#endif /* _PROCESSOR_HPP_ */
