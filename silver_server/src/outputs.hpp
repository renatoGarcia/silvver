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

#ifndef _OUTPUTS_HPP_
#define _OUTPUTS_HPP_

#include "clientsMap.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <fstream>
#include <vector>

/** Implementa o envio das poses desejadas a cada cliente.
 * A classe Saidas mantem um registro de todos os clientes,
 * e a cada novo vetor de estado recebido reenvia para cada cliente a sua nova pose
 */
template <typename Toutput>
class Outputs
{

public:

  Outputs();

  /** Recebe a pose de todos os robôs localizados, processa
   * e envia essa informação para os clientes.
   * @param vecRobo Vetor com todos os marcos localizados. Representados por um objeto da classe Ente.
   */
  void sendResults(const std::vector<Toutput> &vecResults);

private:

  boost::shared_ptr<ClientsMap> clientsMap;

  /// Registrará todas as informações obtidas das cameras
  boost::scoped_ptr<std::ofstream> logFile;

};

#endif /* _OUTPUTS_HPP_ */
