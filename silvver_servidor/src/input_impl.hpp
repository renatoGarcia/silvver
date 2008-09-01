#ifndef INPUTS_IMPL_HPP
#define INPUTS_IMPL_HPP

#include "input.hpp"
#include "processorFactory_impl.hpp"
#include <iostream>
#include <boost/ref.hpp>
#include "silverTypes.hpp"

template <typename Type>
Input<Type>::Input(boost::shared_ptr<Connection> connection,
                     ProcessorType processorType)
  :InputInterface()
  ,connection(connection)
  ,stopping(false)
{
  this->connectionPort = connection->getPort();
  this->processor = ProcessorFactory<Type>::createProcessor(processorType);
}

template <typename Type>
Input<Type>::~Input()
{
  this->stopping = true;
  if(this->runThread)
  {
    this->runThread->join();
  }
}

template <typename Type>
void
Input<Type>::run()
{
  if(!this->runThread)
  {
    this->runThread.reset(new boost::thread(boost::ref(*this)));
  }
}

template <typename Type>
void
Input<Type>::confirmConnect()
{
  char msgOK[3] = "OK";

  this->connection->send(msgOK,sizeof(msgOK));
}

template <typename Type>
void
Input<Type>::operator()()
{
  silver::Package<Type> package;
  std::vector<Type> resultados;
  while(!this->stopping)
  {

    this->connection->receive(&package, sizeof(package));
    this->processor->deliverPackage(package, this->connectionPort);
//     this->processor->localize(resultados);
//     this->saidas->ReceberEstado(resultados);
  }
}

// std::string
// Input::tipoDado2string(DataType td)
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

#endif
