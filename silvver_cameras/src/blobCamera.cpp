#include "blobCamera.hpp"
#include <vector>

BlobCamera::BlobCamera(CameraConfig cameraConfig, std::string serverIP,
                       unsigned connectionPort)
  :AbstractCamera(cameraConfig, serverIP, connectionPort)
{
  this->targetType = AbstractCamera::COLOR_BLOB;

//   this->filtro.reset(new Filtro());
//   this->blobExtractor.reset.(new BlobExtractor());
}

BlobCamera::~BlobCamera()
{}

void
BlobCamera::operator()()
{
  this->connect();

//   this->filtro->Iniciar();

  std::vector<silver::Blob> vecBlob;
  silver::Package<silver::Blob> package;std::cout << "Um\n";
  while(!stopping)
  {std::cout << "dois\n";
    vecBlob.clear();
    this->updateFrame();

//     this->blobExtractor->
  }
  std::cout << "tres\n";
}
