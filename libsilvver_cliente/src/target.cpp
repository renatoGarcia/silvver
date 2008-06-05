#include "target.hpp"
#include <sstream>
#include <iostream>
#include <boost/ref.hpp>

Target::Target(std::string ip, int id, bool log, unsigned receptionistPort)
 :ID_ROBOT(id)
 ,IP_SERVIDOR(ip)
 ,RECEPTIONIST_PORT(receptionistPort)
{
  connection.reset(new Conexao());
  this->mutexActualPose.reset( new boost::mutex() );
  this->finalizarThread = false;

  if(log)
  {
    std::stringstream ssNome;
    ssNome << ID_ROBOT << ".txt";
    arqRegistro.reset( new std::ofstream(ssNome.str().c_str()) );
  }
}

void
Target::makeConnection()
{
  unsigned int connectionPort;
  Conexao receptionistConnection;

  receptionistConnection.Iniciar(this->RECEPTIONIST_PORT,IP_SERVIDOR.c_str());
  receptionistConnection.Enviar((void*)"SD",sizeof("SD") );
  receptionistConnection.Receber((char*)&connectionPort,sizeof(connectionPort));
  std::cout << "Porta: " << connectionPort << std::endl;
  char OK[3]="OK";
  char resposta[3];
  connection->Iniciar(connectionPort,IP_SERVIDOR.c_str());
  connection->Enviar(OK,sizeof(OK));
  connection->Enviar((void*)&ID_ROBOT,sizeof(ID_ROBOT));
  connection->Receber(resposta,sizeof(resposta));
  std::cout << "Resposta: " << resposta << std::endl;
}

void
Target::operator()()
{
  Ente enteReceptor;

  while( !this->finalizarThread )
  {
    try
    {
      this->connection->Receber((char*)&enteReceptor, sizeof(enteReceptor));
    }catch(Conexao::Excecoes excecao){
      if(excecao == Conexao::exc_tempoReceber)
	continue;
    }

    if(enteReceptor.id == this->ID_ROBOT) // Se o id do ente recebido é igual ao do robô.
    {
      boost::mutex::scoped_lock lock( *(this->mutexActualPose) );
      this->actualPose = Pose(enteReceptor.x, enteReceptor.y, enteReceptor.theta);
    }
    else
    {
      std::cerr << "Recebido pacote com ente com id errado." << std::endl
                << "Id recebido: " << enteReceptor.id << std::endl << std::endl;
      continue;
    }

    if(this->arqRegistro)
    {
      *(this->arqRegistro) << enteReceptor.id   << '\t'
                           << enteReceptor.x    << '\t'
                           << enteReceptor.y    << '\t'
                           << enteReceptor.theta << std::endl;
    }
  }
}

void
Target::connect()
{
  makeConnection();
  th.reset(new boost::thread(boost::ref(*this)));
}

void
Target::disconnect()
{
  this->finalizarThread = true;
  th->join();

  char resposta[3];
  char pedido[3] = "DC";
  int id = ID_ROBOT;
  Conexao receptionistConnection;
  receptionistConnection.Iniciar(this->RECEPTIONIST_PORT,IP_SERVIDOR.c_str());
  receptionistConnection.Enviar((void*)pedido,sizeof(pedido) );
  receptionistConnection.Enviar((void*)&id,sizeof(id) );
  receptionistConnection.Receber((char*)resposta,sizeof(resposta));

  std::cout << "Desconectado, resposta: " << resposta << std::endl;
}

Pose
Target::getPose()
{
  boost::mutex::scoped_lock lock( *mutexActualPose );
  return actualPose;
}

void
Target::getPose(double &x,double &y, double &theta)
{
  boost::mutex::scoped_lock lock( *mutexActualPose );
  x    = actualPose.x;
  y    = actualPose.y;
  theta = actualPose.theta;
}
