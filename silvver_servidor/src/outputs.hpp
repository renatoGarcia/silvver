#ifndef OUTPUTS_HPP
#define OUTPUTS_HPP

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

#endif
