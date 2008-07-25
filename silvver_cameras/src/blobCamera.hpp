#ifndef BLOB_CAMERA_HPP
#define BLOB_CAMERA_HPP

#include <string>
#include <boost/scoped_ptr.hpp>
#include "abstractCamera.hpp"
// #include "filtro.hpp"
// #include "blobExtractor.hpp"

class BlobCamera : public AbstractCamera
{
public:

  BlobCamera(CameraConfig camConfig, std::string serverIP,
             unsigned connectionPort);

  virtual ~BlobCamera();

//   int Iniciar();

  virtual void operator()();

private:

//   boost::scoped_ptr<Filtro> filtro;

//   boost::scoped_ptr<BlobExtractor> blobExtractor;
};
#endif
