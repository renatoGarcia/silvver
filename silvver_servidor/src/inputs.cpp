#include "inputs.hpp"
#include <iostream>

Inputs::Inputs()
{}

Inputs::~Inputs()
{}

void
Inputs::addInput(Connection *connection)
{
  char msg[3];
  DataType dataType;

  connection->receive(msg, sizeof(msg));
  std::cout << "Confirma conexao: " << msg << std::endl;

  connection->receive( &dataType,sizeof(DataType) );
  std::cout << "Tipo de dado: " << tipoDado2string(dataType)
            << std::endl << std::endl;

  switch(dataType)
  {
    case BLOB:
//       this->blobControllers.push_back(boost::shared_ptr<BlobCameraController>
//                                       (new BlobCameraController(connection)));
//       blobControllers.back()->run();
      break;
    case MARKER:
      this->markerControllers.push_back(boost::shared_ptr<MarkerCameraController>
                                        (new MarkerCameraController(connection)));
      markerControllers.back()->run();
      break;
    default:
      break;
  }
}

std::string
Inputs::tipoDado2string(DataType td)
{
  std::string str;
  switch(td)
  {
    case BLOB:
      str = "cores";
      break;
    case MARKER:
      str = "marcas";
      break;
    default:
      str = "tipo não identificado";
      break;
  }
  return str;
}
