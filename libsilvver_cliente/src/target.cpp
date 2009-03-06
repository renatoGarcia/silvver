#include "target.hpp"

#include <fstream>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include "connection.ipp"

class Target::CheshireCat
{
public:

  ~CheshireCat();

private:

  friend class Target;

  CheshireCat(unsigned targetId, bool log,
              const std::string& serverIp,
              unsigned receptionistPort);

  /// Envia um pedido de conexão ao recepcionista do Silvver-servidor, e a cria ao receber a resposta.
  void connect();

  /// Stop the thread and close the connection with the receptionist.
  void disconnect();

  void updatePose();

  const unsigned targetId;

  /// Synchronize the write and read in currentPose.
  boost::mutex mutexCurrentPose;

  /// Value of last received Pose.
  silver::Pose currentPose;

  /// Will holds an Ente until convert it safely to currentPose locking
  /// mutexCurrentPose.
  silver::Ente lastEnte;

  boost::scoped_ptr<Connection> connection;

  bool connected;
  bool neverConnected;

  boost::scoped_ptr<std::ofstream> arqRegistro;
};

Target::CheshireCat::CheshireCat(unsigned targetId,
                                 bool log,
                                 const std::string& serverIp,
                                 unsigned receptionistPort)
  :targetId(targetId)
  ,connection(new Connection(serverIp, receptionistPort))
  ,connected(false)
  ,neverConnected(true)
{
  if (log)
  {
    std::string fileName(boost::lexical_cast<std::string>(targetId) + ".log");
    arqRegistro.reset(new std::ofstream(fileName.c_str()));
  }
}

Target::CheshireCat::~CheshireCat()
{
  if (this->connected)
  {
    try
    {
      this->disconnect();
    }
    catch(...)
    {
      std::cerr << "Error on closing Target" << std::endl;
    }
  }
}

void
Target::CheshireCat::connect()
{
  this->connection->connect(this->targetId);
  this->connected = true;

  if (this->neverConnected)
  {
    // This method needs be called just in the first time that connect is
    // called. In the next times that connect is called, the method updatePose
    // alread called the connection->asyncRead.
    this->connection->asyncRead(this->lastEnte,
                                boost::bind(&Target::CheshireCat::updatePose,
                                            this));
    this->neverConnected = false;
  }
}

void
Target::CheshireCat::disconnect()
{
  this->connection->disconnect(this->targetId);
  this->connected = false;
}

void
Target::CheshireCat::updatePose()
{
  if ((unsigned)this->lastEnte.id == this->targetId)
  {
    {
      boost::mutex::scoped_lock lock(this->mutexCurrentPose);
      this->currentPose = silver::Pose(lastEnte.x,
                                       lastEnte.y,
                                       lastEnte.theta);
    }

    if(this->arqRegistro)
    {
      *(this->arqRegistro) << lastEnte.id   << '\t'
                           << lastEnte.x    << '\t'
                           << lastEnte.y    << '\t'
                           << lastEnte.theta << std::endl;
    }
  }

  this->connection->asyncRead(this->lastEnte,
                              boost::bind(&Target::CheshireCat::updatePose,
                                          this));
}

void
Target::connect()
{
  smile->connect();
}

void
Target::disconnect()
{
  smile->disconnect();
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
