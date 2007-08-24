#include "saidas.h"

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

void Saidas::AdicionarSaida(Conexao *conexaoNovoCliente)
{
  char msg[3];
  char OK[3] = "OK";
  int idCliente;

  conexaoNovoCliente->Receber( msg,sizeof(msg) ); //Deverá receber como resposta a mensagem "OK".
  cout << "Confirma conexao: " << msg << endl;

  conexaoNovoCliente->Receber( &idCliente,sizeof(int) );
  cout << "ID: " << idCliente << endl;

  clientes[idCliente] = conexaoNovoCliente;

  clientes.find(idCliente)->second->Enviar(OK,sizeof(OK));
}

void Saidas::ReceberEstado(const vector<Ente> &vecRobo)
{
  mutex::scoped_lock lock(mutexReceberEstado);

  vector<Ente>::const_iterator iteEnte;

  for(iteEnte = vecRobo.begin(); iteEnte < vecRobo.end(); iteEnte++)
  {
    //Caso não exista um cliente com o mesmo id do Ente apontado por iteEnte não faz nada.
    if(clientes.find(iteEnte->id) == clientes.end())
      continue;
    else //Caso contrário envia a localização encontrada para o cliente.
    {
      clientes.find(iteEnte->id)->second->Enviar( (void*)&(*iteEnte),sizeof(*iteEnte) );
    }

    *arqLog << iteEnte->id << '\t' << iteEnte->tempo << '\t'
            << iteEnte->x  << '\t' << iteEnte->y << '\t' << iteEnte->teta <<  endl;
//     cout << iteEnte->id << endl;
  }
}
