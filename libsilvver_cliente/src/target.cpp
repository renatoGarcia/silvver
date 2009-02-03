#include "target.hpp"

#include <iostream>
#include <fstream>

#include <boost/lexical_cast.hpp>
#include <boost/ref.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "conexao.hpp"

class Target::CheshireCat
{
public:

  ~CheshireCat();

private:

  friend class Target;
  friend class boost::thread;

  CheshireCat(unsigned targetId, bool log,
              const std::string& serverIp,
              unsigned receptionistPort);

  /// Envia um pedido de conexão ao recepcionista do Silvver-servidor, e a cria ao receber a resposta.
  void makeConnection();

  /// Stop the thread and close the connection with the receptionist.
  void close();

  /// Método que ficará continuamente esperando pelas mensagens do Silvver-servidor contendo a pose atual.
  /// Ele será executado em uma thread própria.
  void operator()();

  const unsigned targetId;

  /// Synchronize the write and read in currentPose.
  boost::mutex mutexCurrentPose;

  /// Value of last received Pose.
  silver::Pose currentPose;

  boost::scoped_ptr<Connection> receptionist;

  boost::scoped_ptr<Connection> connection;

  boost::scoped_ptr<boost::thread> th;

  /// Sinalizador utilizado para terminar a função ouvirServidor.
  bool stop;

  bool closed;

  boost::scoped_ptr<std::ofstream> arqRegistro;
};

Target::CheshireCat::CheshireCat(unsigned targetId,
                                 bool log,
                                 const std::string& serverIp,
                                 unsigned receptionistPort)
  :targetId(targetId)
  ,receptionist(new Connection(serverIp, receptionistPort))
  ,stop(false)
  ,closed(false)
{
  this->receptionist->connect();

  if(log)
  {
    std::string fileName(boost::lexical_cast<std::string>(targetId) + ".log");
    arqRegistro.reset(new std::ofstream(fileName.c_str()));
  }
}

void Target::CheshireCat::close()
{
  this->stop = true;
  this->th->join();

  char pedido[3] = "DC";
  unsigned connectionPort = connection->getPort();

  receptionist->send((void*)pedido,sizeof(pedido) );
  receptionist->send((void*)&this->targetId,sizeof(this->targetId));
  receptionist->send((void*)&connectionPort,sizeof(connectionPort));

  this->closed = true;
}

Target::CheshireCat::~CheshireCat()
{
  if(!this->closed)
  {
    try
    {
      this->close();
    }
    catch(...)
    {
      std::cerr << "Error on closing Target" << std::endl;
    }
  }
}

void
Target::CheshireCat::makeConnection()
{
  unsigned port;

  receptionist->send((void*)"SD",sizeof("SD") );
  receptionist->receive((char*)&port,sizeof(port));
  std::cout << "Porta: " << port << std::endl;

  char OK[3]="OK";
  char resposta[3];

  this->connection.reset(new Connection(this->receptionist->getPairIP(),
                                        port));
  connection->connect();
  connection->send(OK,sizeof(OK));
  connection->send((void*)&this->targetId,sizeof(this->targetId));
  connection->receive(resposta,sizeof(resposta));
  std::cout << "Resposta: " << resposta << std::endl;
}

void
Target::CheshireCat::operator()()
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

    if((unsigned)enteReceptor.id == this->targetId) // Se o id do ente recebido é igual ao do robô.
    {
      boost::mutex::scoped_lock lock(this->mutexCurrentPose);
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
  smile->makeConnection();
  smile->th.reset(new boost::thread(boost::ref(*smile)));
}

silver::Pose
Target::getPose()
{
  boost::mutex::scoped_lock lock(smile->mutexCurrentPose);
  return smile->currentPose;
}

void
Target::getPose(double &x,double &y, double &theta)
{
  boost::mutex::scoped_lock lock(smile->mutexCurrentPose);
  x     = smile->currentPose.x;
  y     = smile->currentPose.y;
  theta = smile->currentPose.theta;
}

Target::Target(unsigned targetId,
               bool log,
               const std::string& serverIp,
               unsigned receptionistPort)
  :smile(new CheshireCat(targetId, log, serverIp, receptionistPort))
{}

Target::~Target() throw()
{}
