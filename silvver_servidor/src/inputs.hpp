#ifndef INPUTS_HPP
#define INPUTS_HPP

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include "connection.hpp"
#include "saidas.hpp"

class Inputs
{
public:

  Inputs(boost::shared_ptr<Connection> connection);

  virtual ~Inputs();

  void confirmConnect();

  void run();

  virtual void operator()()=0;

protected:

  boost::shared_ptr<Connection> connection;

  bool stopping;

  boost::scoped_ptr<boost::thread> runThread;

  unsigned connectionPort;

  Saidas *saidas;

};

#endif
