#include "blobCamera.hpp"
#include <vector>
#include "xmlParser.hpp"

BlobCamera::BlobCamera(const std::vector<TargetConfig> &vecTargets,
                       CameraConfig cameraConfig,
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
