#ifndef BLOB_CAMERA_HPP
#define BLOB_CAMERA_HPP

#include <string>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "abstractCamera.hpp"
#include "connection.hpp"
#include "blobExtractor.hpp"

class BlobCamera : public AbstractCamera
{
public:

  BlobCamera(const std::vector<scene::Target> &vecTargets,
             scene::Camera camConfig,
             boost::shared_ptr<Connection> connection);

  virtual ~BlobCamera();

//   int Iniciar();

  virtual void operator()();

private:

  boost::scoped_ptr<ColorBlobExtractor> colorBlobExtractor;
};
#endif
