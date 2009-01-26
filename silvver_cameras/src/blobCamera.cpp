#include "blobCamera.hpp"
#include <vector>
#include "scene.hpp"

BlobCamera::BlobCamera(const std::vector<scene::Target> &vecTargets,
                       scene::Camera cameraConfig,
                       std::string serverIP,
                       unsigned connectionPort)
  :AbstractCamera(cameraConfig, serverIP, connectionPort)
{
  this->targetType = AbstractCamera::COLOR_BLOB;

  this->colorBlobExtractor.reset(new ColorBlobExtractor(vecTargets));
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

    this->colorBlobExtractor->extract(this->currentFrame, vecBlob);
  }
  std::cout << "tres\n";
}
