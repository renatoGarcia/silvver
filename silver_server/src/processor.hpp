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

#include "processorInterface.hpp"

#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

#include "clientsMap.hpp"

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

  typedef std::map< unsigned, std::vector<Tinput> > TMap;

  // Armazenara os ultimos entes obtidos de cada marcaCamera, onde
  // a chave e um identificador destas.

  /// Hold the last input of each input client.
  TMap lastInputs;

private:

  boost::shared_ptr<ClientsMap> clientsMap;
};

#endif /* _PROCESSOR_HPP_ */
