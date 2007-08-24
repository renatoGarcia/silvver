#ifndef SAIDAS_H
#define SAIDAS_H

#include <map>
#include "tipos.h"
#include <boost/thread/mutex.hpp>
#include "conexao.h"
#include <fstream>

using namespace std;
using namespace boost;
using namespace verlab;

class Saidas
{
private:

  static auto_ptr<Saidas> instanciaUnica;
  static mutex mutexInstanciacao;
  Saidas();

  mutex mutexReceberEstado;

  // Socket de comunicação com os clientes que receberão a sua própria pose.
  // A chave int é o id de interesse.
  map< int,Conexao* > clientes;

  // Registrará todas as informações obtidas das cameras
  ofstream *arqLog;

public:

  static Saidas* Instanciar();

  void AdicionarSaida(Conexao *conexaoNovoCliente);

  // Recebe um vector com a pose de todos os rob�s localizados, e processa
  // e envia essa informação para os clientes.
  void ReceberEstado(const vector<Ente> &vecRobo);
};

#endif
