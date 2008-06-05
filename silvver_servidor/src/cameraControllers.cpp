#include "cameraControllers.hpp"
#include <vector>

//----------------------MARKER_CAMERA------------------------------//

MarkerCameraController::MarkerCameraController(Connection *connection)
{
  char msgOK[3] = "OK";

  this->processor = MarkerProcessor::instantiate();
  this->connection.reset(connection);

  this->connection->send(msgOK,sizeof(msgOK));

  this->connectionPort = connection->getPort();

  this->saidas = Saidas::Instanciar();

  this->stopping = false;
}

MarkerCameraController::~MarkerCameraController()
{
  this->stopping = true;
  this->th->join();
}

void
MarkerCameraController::run()
{
  if(!this->th)
  {
    th.reset(new boost::thread(boost::ref(*this)));
  }
}

void
MarkerCameraController::operator()()
{
  Package<Ente> package;
  std::vector<Ente> resultados;
  while(!stopping)
  {
    this->connection->receive( &package,sizeof(package) );
    this->processor->deliverPackage(package, this->connectionPort);
    this->processor->localize(resultados);
    this->saidas->ReceberEstado(resultados);
  }
}

//----------------------BLOB_CAMERA------------------------------//

BlobCameraController::BlobCameraController(Connection *connection)
{
  char msgOK[3] = "OK";

  this->processor = BlobTratador::Instanciar();
  this->connection.reset(connection);

  this->connection->send(msgOK,sizeof(msgOK));

  this->connectionPort = connection->getPort();

  this->saidas = Saidas::Instanciar();

  this->stopping = false;
}

BlobCameraController::~BlobCameraController()
{
  this->stopping = true;
  this->th->join();
}

void
BlobCameraController::run()
{
  if(!this->th)
  {
    th.reset(new boost::thread(boost::ref(*this)));
  }
}

void
BlobCameraController::operator()()
{
  Package<Blob> package;
  std::vector<Ente> resultados;
  while(!stopping)
  {
    this->connection->receive( &package,sizeof(package) );
    this->processor->EntregarPacotes(package, this->connectionPort);
    //saidas->
  }
}
