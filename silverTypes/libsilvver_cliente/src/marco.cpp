#include "marco.hpp"
#include <sstream>
#include <iostream>
#include <boost/bind.hpp>

Marco::Marco(string ip, int id, bool registrar)
:ID_ROBO(id)
,IP_SERVIDOR(ip)
{
  conexao.reset(new Conexao());
  this->mutexUltimaPose.reset( new boost::mutex() );
  this->finalizarThread = false;

  if(registrar)
  {
    stringstream ssNome;
    ssNome << ID_ROBO << ".txt";
    arqRegistro.reset( new ofstream(ssNome.str().c_str()) );
  }
}

void Marco::criarConexao()
{
  unsigned int portaConexao;
  Conexao conexaoRecepcionista;

  conexaoRecepcionista.Iniciar(PORTA_RECEPCIONISTA,IP_SERVIDOR.c_str());
  conexaoRecepcionista.Enviar((void*)"SD",sizeof("SD") );
  conexaoRecepcionista.Receber((char*)&portaConexao,sizeof(portaConexao));
  cout << "Porta: " << portaConexao << endl;
  char OK[3]="OK";
  char resposta[3];
  conexao->Iniciar(portaConexao,IP_SERVIDOR.c_str());
  conexao->Enviar(OK,sizeof(OK));
  conexao->Enviar((void*)&ID_ROBO,sizeof(ID_ROBO));
  conexao->Receber(resposta,sizeof(resposta));
  cout << "Resposta: " << resposta << endl;
}

void Marco::ouvirServidor(Marco *objeto)
{
  Ente enteReceptor;

  while( !objeto->finalizarThread )
  {
    try
    {
      objeto->conexao->Receber((char*)&enteReceptor, sizeof(enteReceptor));
    }catch(Conexao::Excecoes excecao){
      if(excecao == Conexao::exc_tempoReceber)
	continue;
    }

    if(enteReceptor.id == objeto->ID_ROBO) // Se o id do ente recebido é igual ao do robô.
    {
      boost::mutex::scoped_lock lock( *(objeto->mutexUltimaPose) );
      objeto->ultimaPose = Pose(enteReceptor.x, enteReceptor.y, enteReceptor.teta);
    }
    else
    {
      cerr << "Recebido pacote com ente com id errado." << endl
	   << "Id recebido: " << enteReceptor.id << endl << endl;
      continue;
    }

    if(objeto->arqRegistro)
    {
      *(objeto->arqRegistro) << enteReceptor.id   << '\t' << enteReceptor.tempo << '\t'
                             << enteReceptor.x    << '\t' << enteReceptor.y     << '\t'
                             << enteReceptor.teta << endl;
    }
  }
}

void Marco::conectar()
{
  criarConexao();
  thOuvirServidor.reset(new boost::thread( boost::bind(&Marco::ouvirServidor,this) ));
  //thOuvirServidor.reset( new thread( *this ) );
}

void Marco::desconectar()
{
  this->finalizarThread = true;
  thOuvirServidor->join();

  char resposta[3];
  char pedido[3] = "DC";
  int id = ID_ROBO;
  Conexao conexaoRecepcionista;
  conexaoRecepcionista.Iniciar(PORTA_RECEPCIONISTA,IP_SERVIDOR.c_str());
  conexaoRecepcionista.Enviar((void*)pedido,sizeof(pedido) );
  conexaoRecepcionista.Enviar((void*)&id,sizeof(id) );
  conexaoRecepcionista.Receber((char*)resposta,sizeof(resposta));

  cout << "Desconectado, resposta: " << resposta << endl;
}

Pose Marco::getPose()
{
  boost::mutex::scoped_lock lock( *mutexUltimaPose );
  return ultimaPose;
}

void Marco::getPose(double &x,double &y, double &teta)
{
  boost::mutex::scoped_lock lock( *mutexUltimaPose );
  x    = ultimaPose.x;
  y    = ultimaPose.y;
  teta = ultimaPose.teta;
}
