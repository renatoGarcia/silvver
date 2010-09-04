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

#ifndef _PROCESSOR_INTERFACE_HPP_
#define _PROCESSOR_INTERFACE_HPP_

#include "common/silvverTypes.hpp"

/// Interface to access a Processor class withou know the output type.
template <class Tinput>
class ProcessorInterface
{
public:
  /** Deliver data to be processed.
   * This method must be implemented by a concrete processor class.
   * @param reading A reading of a camera.
   * @param inputUid A key unique to calling input. */
  virtual void deliverPackage(silvver::CameraReading<Tinput>& reading)=0;

  virtual ~ProcessorInterface()
  {}

protected:
  ProcessorInterface()
  {}
};

#endif /* _PROCESSOR_INTERFACE_HPP_ */
