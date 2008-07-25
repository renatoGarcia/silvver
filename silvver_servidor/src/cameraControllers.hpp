#ifndef CAMERA_CONTROLLERS_HPP
#define CAMERA_CONTROLLERS_HPP

#include <boost/ref.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include "connection.hpp"
#include "silverTypes.hpp"
#include "saidas.hpp"
// #include "blobTratador.hpp"
#include "markerProcessor.hpp"

using namespace silver;

class MarkerCameraController
{
public:

  MarkerCameraController(Connection* connection);

  ~MarkerCameraController();

  void run();

  void operator()();

private:
  bool stopping;

  boost::scoped_ptr<boost::thread> th;

  boost::scoped_ptr<Connection> connection;

  unsigned connectionPort;

  MarkerProcessor *processor;

  Saidas *saidas;
};

//-----------------------------------------------------------

// class BlobCameraController
// {
// public:

//   BlobCameraController(Connection* connection);

//   ~BlobCameraController();

//   void run();

//   void operator()();

// private:
//   bool stopping;

//   boost::scoped_ptr<boost::thread> th;

//   boost::scoped_ptr<Connection> connection;

//   unsigned connectionPort;

//   BlobTratador *processor;

//   Saidas *saidas;
// };

#endif
