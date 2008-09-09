#include "target.hpp"
#include <iostream>
#include <boost/ref.hpp>
#include <boost/lexical_cast.hpp>

Target::Target(std::string serverIp, unsigned targetId,
               bool log, unsigned receptionistPort)
  :targetId(targetId)
  ,serverIp(serverIp)
  ,receptionistPort(receptionistPort)
  ,stop(false)
{
  this->mutexCurrentPose.reset( new boost::mutex() );

  if(log)
  {
    std::string fileName(boost::lexical_cast<std::string>(targetId) + ".log");
    arqRegistro.reset(new std::ofstream(fileName.c_str()));
  }
}

Target::~Target()
{
  this->stop = true;
  th->join();

  char pedido[3] = "DC";
  unsigned connectionPort = connection->getPort();

  receptionistConnection->send((void*)pedido,sizeof(pedido) );
  receptionistConnection->send((void*)&this->targetId,sizeof(this->targetId));
  receptionistConnection->send((void*)&connectionPort,sizeof(connectionPort));

  std::cout << "Desconectado" << std::endl;
}


void
Target::makeConnection()
{
  this->receptionistConnection.reset(new Connection(this->serverIp,
                                                    this->receptionistPort));

  unsigned port;

  receptionistConnection->connect();
  receptionistConnection->send((void*)"SD",sizeof("SD") );
  receptionistConnection->receive((char*)&port,sizeof(port));
  std::cout << "Porta: " << port << std::endl;

  char OK[3]="OK";
  char resposta[3];

  this->connection.reset(new Connection(this->serverIp, port));
  connection->connect();
  connection->send(OK,sizeof(OK));
  connection->send((void*)&this->targetId,sizeof(this->targetId));
  connection->receive(resposta,sizeof(resposta));
  std::cout << "Resposta: " << resposta << std::endl;
}

void
Target::operator()()
{
  silver::Ente enteReceptor;

  while(!this->stop)
  {
    try
    {
      this->connection->receive((char*)&enteReceptor, sizeof(enteReceptor));
    }catch(Connection::Excecoes excecao){
      if(excecao == Connection::exc_tempoReceber)
	continue;
    }

    if(enteReceptor.id == this->targetId) // Se o id do ente recebido é igual ao do robô.
    {
      boost::mutex::scoped_lock lock( *(this->mutexCurrentPose) );
      this->currentPose = silver::Pose(enteReceptor.x,
                                       enteReceptor.y,
                                       enteReceptor.theta);
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

Pose
Target::getPose()
{
  boost::mutex::scoped_lock lock( *mutexCurrentPose );
  return currentPose;
}

void
Target::getPose(double &x,double &y, double &theta)
{
  boost::mutex::scoped_lock lock( *mutexCurrentPose );
  x     = currentPose.x;
  y     = currentPose.y;
  theta = currentPose.theta;
}
