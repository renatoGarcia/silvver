#include "inputs.hpp"
#include <iostream>
#include <boost/ref.hpp>

Inputs::Inputs(boost::shared_ptr<Connection> connection)
  :connection(connection)
  ,stopping(false)
{
  this->saidas = Saidas::Instanciar();
  this->connectionPort = connection->getPort();
}

Inputs::~Inputs()
{
  this->stopping = true;
  if(this->runThread)
  {
    this->runThread->join();
  }
}

void
Inputs::run()
{
  if(!this->runThread)
  {
    this->runThread.reset(new boost::thread(boost::ref(*this)));
  }
}

void
Inputs::confirmConnect()
{
  char msgOK[3] = "OK";

  this->connection->send(msgOK,sizeof(msgOK));
}

// std::string
// Inputs::tipoDado2string(DataType td)
// {
//   std::string str;
//   switch(td)
//   {
//     case COLOR_BLOB:
//       str = "cores";
//       break;
//     case ARTP_MARK:
//       str = "marcas";
//       break;
//     default:
//       str = "tipo não identificado";
//       break;
//   }
//   return str;
// }
