#include "target.hpp"

#include <fstream>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/condition.hpp>
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

  boost::condition_variable newPoseCondition;

  /// Value of last received Pose.
  silver::Pose currentPose;

  // Signalize a never returned/read currentPose.
  bool currentPoseIsNew;

  /// Will holds an Ente until convert it safely to currentPose locking
  /// mutexCurrentPose.
  silver::Ente lastEnte;

  boost::scoped_ptr<Connection> connection;

  bool connected;

  boost::scoped_ptr<std::ofstream> arqRegistro;
};

Target::CheshireCat::CheshireCat(unsigned targetId,
                                 bool log,
                                 const std::string& serverIp,
                                 unsigned receptionistPort)
  :targetId(targetId)
  ,currentPoseIsNew(false)
  ,connection(new Connection(serverIp, receptionistPort))
  ,connected(false)
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

  this->connection->asyncRead(this->lastEnte,
                                boost::bind(&Target::CheshireCat::updatePose,
                                            this));
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

    this->currentPoseIsNew = true;
    this->newPoseCondition.notify_one();

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

unsigned
Target::getId()
{
  return smile->targetId;
}

silver::Pose
Target::getLastPose()
{
  boost::mutex::scoped_lock lock(smile->mutexCurrentPose);

  smile->currentPoseIsNew = false;
  return smile->currentPose;
}

void
Target::getLastPose(double &x,double &y, double &theta)
{
  boost::mutex::scoped_lock lock(smile->mutexCurrentPose);

  smile->currentPoseIsNew = false;
  x     = smile->currentPose.x;
  y     = smile->currentPose.y;
  theta = smile->currentPose.theta;
}

silver::Pose
Target::getNewPose()
{
  boost::mutex::scoped_lock lock(smile->mutexCurrentPose);

  if (!smile->currentPoseIsNew)
  {
    throw Target::old_pose_error("The current pose was read already");
  }

  smile->currentPoseIsNew = false;
  return smile->currentPose;
}

void
Target::getNewPose(double &x,double &y, double &theta)
{
  boost::mutex::scoped_lock lock(smile->mutexCurrentPose);

  if (!smile->currentPoseIsNew)
  {
    throw Target::old_pose_error("The current pose was read already");
  }

  smile->currentPoseIsNew = false;
  x     = smile->currentPose.x;
  y     = smile->currentPose.y;
  theta = smile->currentPose.theta;
}

silver::Pose
Target::getNextPose()
{
  boost::mutex::scoped_lock lock(smile->mutexCurrentPose);

  // Wait for updatePose function notifies a new pose.
  smile->newPoseCondition.wait(lock);

  smile->currentPoseIsNew = false;
  return smile->currentPose;
}

void
Target::getNextPose(double &x,double &y, double &theta)
{
  boost::mutex::scoped_lock lock(smile->mutexCurrentPose);

  // Wait for updatePose function notifies a new pose.
  smile->newPoseCondition.wait(lock);

  smile->currentPoseIsNew = false;
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
