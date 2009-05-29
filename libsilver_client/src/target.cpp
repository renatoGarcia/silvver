#include "target.hpp"

#include <fstream>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include "connection.ipp"

namespace bpt = boost::posix_time;

namespace silver
{

template<class U>
class
CheshireCat
{
public:

  ~CheshireCat();

private:

  friend class Target<U>;

  CheshireCat(unsigned targetId, bool log,
              const std::string& serverIp,
              unsigned receptionistPort);

  /// Envia um pedido de conexão ao recepcionista do Silvver-servidor, e a cria ao receber a resposta.
  void connect();

  /// Stop the thread and close the connection with the receptionist.
  void disconnect();

  void update();

  const unsigned targetId;

  /// Synchronize the write and read in current.
  boost::mutex mutexCurrentPose;

  boost::condition_variable newPoseCondition;

  /// Value of last received Pose.
  Identity<U> current;

  // Signalize a never returned/read current.
  bool currentIsNew;

  /// Will holds an Ente until convert it safely to
  /// current locking mutexCurrentPose.
  Identity<U> last;

  boost::scoped_ptr<Connection> connection;

  bool connected;

  boost::scoped_ptr<std::ofstream> arqRegistro;
};

template<class U>
CheshireCat<U>::CheshireCat(unsigned targetId,
                            bool log,
                            const std::string& serverIp,
                            unsigned receptionistPort)
  :targetId(targetId)
  ,currentIsNew(false)
  ,connection(new Connection(serverIp, receptionistPort))
  ,connected(false)
{
  if (log)
  {
    std::string fileName(boost::lexical_cast<std::string>(targetId) + ".log");
    arqRegistro.reset(new std::ofstream(fileName.c_str()));
  }
}

template<class U>
CheshireCat<U>::~CheshireCat()
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

template<class U>
void
CheshireCat<U>::connect()
{
  this->connection->connect(this->targetId);
  this->connected = true;

  this->connection->asyncRead(this->last,
                              boost::bind(&CheshireCat<U>::update,
                                          this));
}

template<class U>
void
CheshireCat<U>::disconnect()
{
  this->connection->disconnect(this->targetId);
  this->connected = false;
}

template<class U>
void
CheshireCat<U>::update()
{
  if ((unsigned)this->last.uid == this->targetId)
  {
    {
      boost::mutex::scoped_lock lock(this->mutexCurrentPose);
      this->current = this->last;

      this->currentIsNew = true;
    }

    this->newPoseCondition.notify_one();

    if(this->arqRegistro)
    {
      *(this->arqRegistro) << last.uid   << '\t'
                           << last.x    << '\t'
                           << last.y    << '\t'
                           << last.yaw << std::endl;
    }
  }

  this->connection->asyncRead(this->last,
                              boost::bind(&CheshireCat<U>::update,
                                          this));
}

template<class T>
void
Target<T>::connect()
{
  smile->connect();
}

template<class T>
void
Target<T>::disconnect()
{
  smile->disconnect();
}

template<class T>
unsigned
Target<T>::getId()
{
  return smile->targetId;
}

template<class T>
Identity<T>
Target<T>::getLast()
{
  boost::mutex::scoped_lock lock(smile->mutexCurrentPose);

  smile->currentIsNew = false;
  return smile->current;
}

template<class T>
Identity<T>
Target<T>::getNew(const boost::posix_time::time_duration& waitTime)
{
  boost::mutex::scoped_lock lock(smile->mutexCurrentPose);

  while (!smile->currentIsNew)
  {
    if (!smile->newPoseCondition.timed_wait(lock,
                                            bpt::second_clock::universal_time() +
                                            waitTime))
    {
      throw typename
        Target<T>::time_expired_error("The wait time expired without a new "
                                      "pose arrives");
    }
  }

  smile->currentIsNew = false;
  return smile->current;
}

template<class T>
Identity<T>
Target<T>::getNext(const boost::posix_time::time_duration& waitTime)
{
  boost::mutex::scoped_lock lock(smile->mutexCurrentPose);

  // Wait for update function notifies a new pose.
  if (!smile->newPoseCondition.timed_wait(lock,
                                          bpt::second_clock::universal_time() +
                                          waitTime))
  {
    throw typename
      Target<T>::time_expired_error("The wait time expired without the next "
                                    "pose arrives");
  }

  smile->currentIsNew = false;
  return smile->current;
}

template<class T>
Target<T>::Target(unsigned targetId,
               bool log,
               const std::string& serverIp,
               unsigned receptionistPort)
  :smile(new CheshireCat<T>(targetId, log, serverIp, receptionistPort))
{}

template<class T>
Target<T>::~Target() throw()
{}

template class Target<Pose>;

} // End namespace silver
