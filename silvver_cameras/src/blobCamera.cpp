#include "blobCamera.hpp"
#include <vector>
#include "scene.hpp"

BlobCamera::BlobCamera(const std::vector<scene::Target> &vecTargets,
                       scene::Camera cameraConfig,
                       boost::shared_ptr<Connection> connection)
  :AbstractCamera(cameraConfig, connection)
{
  this->colorBlobExtractor.reset(new ColorBlobExtractor(vecTargets));
}

BlobCamera::~BlobCamera()
{}

void
BlobCamera::operator()()
{
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
