#include "blobCamera.hpp"
#include <vector>

BlobCamera::BlobCamera(CameraConfig cameraConfig, std::string serverIP,
                       unsigned connectionPort)
  :AbstractCamera(cameraConfig, serverIP, connectionPort)
{
  this->targetType = AbstractCamera::COLOR_BLOB;

  this->colorBlobExtractor.reset(new ColorBlobExtractor());
}

BlobCamera::~BlobCamera()
{}

void
BlobCamera::operator()()
{
  this->connect();

  std::vector<silver::Blob> vecBlob;
  silver::Package<silver::Blob> package;
  while(!this->stopping)
  {
    vecBlob.clear();
    this->updateFrame();

    this->colorBlobExtractor->extract(this->actualFrame, vecBlob);
  }
  std::cout << "tres\n";
}
