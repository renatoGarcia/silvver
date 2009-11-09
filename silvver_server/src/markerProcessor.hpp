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

#ifndef MARKER_PROCESSOR_HPP
#define MARKER_PROCESSOR_HPP

#include <map>
#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include "silvverTypes.hpp"
#include "processor.hpp"
#include <singleton.hpp>

class MarkerProcessor :  public Processor<silvver::Identity<silvver::Pose>,
                                          silvver::Identity<silvver::Pose> >,
                         public Singleton<MarkerProcessor>

{
public:

  void deliverPackage(const std::vector< silvver::Identity<silvver::Pose> > &pacote,
                      const unsigned id);

  // Calcula a configuraçãoo dos robôs, usando os dados atualmente
  // disponíveis no map armazenador.
  void localize(/*std::vector<silvver::Ente> &vecRobos*/);

private:

  friend class Singleton<MarkerProcessor>;

  MarkerProcessor();

  boost::mutex mutexArmazenador;
};

#endif
