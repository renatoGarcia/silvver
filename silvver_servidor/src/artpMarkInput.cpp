#include "artpMarkInput.hpp"
#include "silverTypes.hpp"

ARTPMarkInput::ARTPMarkInput(boost::shared_ptr<Connection> connection)
  :Inputs(connection)
{
  this->processor = MarkerProcessor::instantiate();
}

ARTPMarkInput::~ARTPMarkInput()
{}

void
ARTPMarkInput::operator()()
{
  Package<Ente> package;
  std::vector<Ente> resultados;
  while(!this->stopping)
  {
    this->connection->receive( &package,sizeof(package) );
    this->processor->deliverPackage(package, this->connectionPort);
    this->processor->localize(resultados);
    this->saidas->ReceberEstado(resultados);
  }
}
