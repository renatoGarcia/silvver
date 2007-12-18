#include "marco.h"
#include <sstream>
#include <iostream>
#include <boost/bind.hpp>

Marco::Marco(string ip, int id, bool registrar):
ID_ROBO(id),
IP(ip)
{
  conexao = new Conexao();
  this->registrar = registrar;
  this->finalizarThread = false;
}

Marco::~Marco()
{
  this->desconectar();
  delete conexao;
  delete arqRegistro;
  delete thOuvirServidor;
}

void Marco::criarConexao()
{
  unsigned int portaConexao;
  Conexao conexaoRecepcionista;

  conexaoRecepcionista.Iniciar(PORTA_RECEPCIONISTA,IP.c_str());
  conexaoRecepcionista.Enviar((void*)"SD",sizeof("SD") );
  conexaoRecepcionista.Receber((char*)&portaConexao,sizeof(portaConexao));
  cout << "Porta: " << portaConexao << endl;
  char OK[3]="OK";
  char resposta[3];
  conexao->Iniciar(portaConexao,IP.c_str());
  conexao->Enviar(OK,sizeof(OK));
  conexao->Enviar((void*)&ID_ROBO,sizeof(ID_ROBO));
  conexao->Receber(resposta,sizeof(resposta));
  cout << "Resposta: " << resposta << endl;

  if (registrar)
  {
    stringstream ssNome;
    ssNome << ID_ROBO << ".txt";
    arqRegistro = new ofstream(ssNome.str().c_str());
  }
}

void Marco::ouvirServidor(Marco *objeto)
{
  Ente enteReceptor;

  while( !objeto->finalizarThread )
  {
    objeto->conexao->Receber((char*)&enteReceptor, sizeof(enteReceptor));

    if(enteReceptor.id == objeto->ID_ROBO) // Se o id do ente recebido é igual ao do robô.
    {
      mutex::scoped_lock lock(objeto->mutexUltimaPose);
      objeto->ultimaPose = Pose(enteReceptor.x, enteReceptor.y, enteReceptor.teta);
    }
    else
    {
      cerr << "Recebido pacote com ente com id errado." << endl
          << "Id recebido: " << enteReceptor.id << endl << endl;
      continue;
    }

    if(objeto->registrar)
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

  thOuvirServidor= new thread( bind(&Marco::ouvirServidor,this) );
}

void Marco::desconectar()
{
  this->finalizarThread = true;
  thOuvirServidor->join();

  char resposta[3];
  Conexao conexaoRecepcionista;
  conexaoRecepcionista.Iniciar(PORTA_RECEPCIONISTA,IP.c_str());
  conexaoRecepcionista.Enviar((void*)"DC",sizeof("DC") );
  conexaoRecepcionista.Enviar((void*)ID_ROBO,sizeof(ID_ROBO) );
  conexaoRecepcionista.Receber((char*)&resposta,sizeof(resposta));
}

Pose Marco::getPose()
{
  return ultimaPose;
}

void Marco::getPose(double &x,double &y, double &teta)
{
  x    = ultimaPose.x;
  y    = ultimaPose.y;
  teta = ultimaPose.teta;
}
