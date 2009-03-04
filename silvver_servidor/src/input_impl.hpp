#ifndef INPUTS_IMPL_HPP
#define INPUTS_IMPL_HPP

#include "input.hpp"
#include "processorFactory_impl.hpp"
#include <iostream>
#include <vector>
#include <boost/bind.hpp>
#include "silverTypes.hpp"

template <typename Type>
Input<Type>::Input(boost::shared_ptr<IoConnection> connection,
                   ProcessorType processorType)
  :InputInterface()
  ,connection(connection)
  ,connectionPort(connection->getLocalPort())
  ,processor(ProcessorFactory<Type>::createProcessor(processorType))
{
  this->connection->asyncReceive(this->inputs,
                                 boost::bind(&Input<Type>::handleReceive,
                                             this));
}

template <typename Type>
Input<Type>::~Input()
{}

template <typename Type>
void
Input<Type>::handleReceive()
{
  this->processor->deliverPackage(this->inputs, this->connectionPort);

  this->connection->asyncReceive(this->inputs,
                                 boost::bind(&Input<Type>::handleReceive,
                                             this));
}

#endif
