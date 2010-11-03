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

#ifndef _PROCESSOR_HPP_
#define _PROCESSOR_HPP_

#include <boost/shared_ptr.hpp>
#include <vector>

#include "common/connection/channel.hpp"
#include "common/processorOptions.hpp"
#include "common/silvverTypes.hpp"
#include "clientMultiMap.hpp"
#include "processorBase.hpp"

/// Abstract base class to the concrete processor classes.
template <class Tinput>
class Processor
  :public ProcessorBase
{
public:
  /** Deliver data to be processed.
   * This method must be implemented by a concrete processor class.
   * @param reading A reading of an AbstractCamera.
   */
  virtual
  void deliverPackage(silvver::CameraReading<Tinput>& reading) = 0;

  virtual
  bool isSameSpec(const procOpt::AnyProcOpt& spec) const;

protected:
  typedef boost::shared_ptr<connection::Channel> ChannelPointer;

  Processor(const procOpt::AnyProcOpt& spec,
            const silvver::TargetSetUid& targetSetUid);

  /** Send the final localizations to clients hearing for it.
   *
   * @param localizations A vector with all target localized.  */
  template <class Toutput>
  void sendToOutputs(const std::vector<silvver::Identity<Toutput> >& localizations) const;

private:
  boost::shared_ptr<ClientMultiMap<silvver::TargetUid> > targetClients;

  boost::shared_ptr<ClientMultiMap<silvver::TargetSetUid> > targetSetClients;

  const procOpt::AnyProcOpt processorSpec;

  const silvver::TargetSetUid targetSetUid;
};

#endif /* _PROCESSOR_HPP_ */
