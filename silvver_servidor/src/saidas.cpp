#include "saidas.hpp"
#include <iostream>

auto_ptr<Saidas> Saidas::instanciaUnica;
mutex Saidas::mutexInstanciacao;

Saidas* Saidas::Instanciar()
{
  mutex::scoped_lock lock(mutexInstanciacao);
  if( instanciaUnica.get()==0 )
      instanciaUnica.reset(new Saidas);

  return instanciaUnica.get();
}

Saidas::Saidas()
{
  arqLog = new ofstream("log.txt");
}

void Saidas::AdicionarSaida(Connection *conexaoNovoCliente)
{
  char msg[3];
  char OK[3] = "OK";
  int idCliente;

  conexaoNovoCliente->receive( msg,sizeof(msg) ); //Deverá receber como resposta a mensagem "OK".
  std::cout << "Confirma conexao: " << msg << std::endl;

  conexaoNovoCliente->receive( &idCliente,sizeof(int) );
  std::cout << "ID: " << idCliente << std::endl;

  clientes[idCliente] = conexaoNovoCliente;

  clientes.find(idCliente)->second->send(OK,sizeof(OK));
}

void Saidas::ReceberEstado(const vector<Ente> &vecRobo)
{
  mutex::scoped_lock lock(mutexReceberEstado);

  vector<Ente>::const_iterator iteEnte;

  for(iteEnte = vecRobo.begin(); iteEnte < vecRobo.end(); iteEnte++)
  {
    *arqLog << iteEnte->id << '\t' << iteEnte->tempo << '\t'
            << iteEnte->x  << '\t' << iteEnte->y << '\t' << iteEnte->teta <<  endl;

    //Caso exista um cliente com o mesmo id do Ente apontado por iteEnte envia a pose para ele.
    if(clientes.find(iteEnte->id) != clientes.end())
      clientes.find(iteEnte->id)->second->send( (void*)&(*iteEnte),sizeof(*iteEnte) );
  }
}

void Saidas::RetirarSaida(int id)
{
  clientes.erase( clientes.find(id) );
}
