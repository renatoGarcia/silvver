#include "colorBlobInput.hpp"
#include "silverTypes.hpp"

ColorBlobInput::ColorBlobInput(boost::shared_ptr<Connection> connection)
  :Inputs(connection)
{
}

ColorBlobInput::~ColorBlobInput()
{}

void
ColorBlobInput::operator()()
{
//   Package<Blob> package;
//   std::vector<Ente> resultados;
//   while(!stopping)
//   {
//     this->connection->receive( &package,sizeof(package) );
//     this->processor->EntregarPacotes(package, this->connectionPort);
//     //saidas->
//   }
}
