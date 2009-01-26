#include "abstractCamera.hpp"
#include "hardCameraFactory.hpp"
#include <boost/ref.hpp>
#include <boost/thread/mutex.hpp>
#include <fstream>
#include "tsPrint.hpp"

AbstractCamera::AbstractCamera(const scene::Camera& cameraConfig,
                               const std::string& serverIP,
                               const unsigned connectionPort)
{

//   this->cameraConfig = cameraConfig;
  if (const scene::MatrixHomography* const matrixHomography =
      boost::get<const scene::MatrixHomography>(&cameraConfig.homography))
  {
    this->homography.reset(new Matrix(*matrixHomography));
  }
  else if (const scene::LutHomography* const lutHomography =
           boost::get<const scene::LutHomography>(&cameraConfig.homography))
  {
    this->homography.reset(new Lut(*lutHomography));
  }

  this->frameCounter = 0;
  this->frameRate = 0;

  this->UID = cameraConfig.uid;

  this->connection.reset(new Connection(serverIP, connectionPort));

  this->stopping = false;

  this->hardCamera = HardCameraFactory::createHardCamera(cameraConfig);

  this->hardCamera->createIplImage(currentFrame);
}

AbstractCamera::~AbstractCamera()
{
  cvReleaseImage(&currentFrame);
}

void
AbstractCamera::connect()
{
  this->connection->connect();

  // Envia a segunda mensagem ao recepcionista("OK"), informando que
  // a conexao foi criada
  char msgOK[3] = "OK";
  this->connection->send(msgOK,sizeof(msgOK));

  // Envia a terceira mensagem ao recepcionista, informando o código
  // do dado que será transmitido
  int tipoDado = this->targetType;
  this->connection->send(&tipoDado,sizeof(tipoDado));

  char msg[3];
  // Espera uma mensagem de confirmação
  this->connection->receive(msg,sizeof(msg));

  {PRINT_LOCK;
    std::cout << "Conecção câmera: " << this->UID
              << " " << msg << std::endl;}
}

void
AbstractCamera::run()
{
  this->runThread.reset(new boost::thread(boost::ref(*this)));
}

void
AbstractCamera::stop()
{
  this->stopping = true;
  this->runThread->join();
}

void
AbstractCamera::updateFrame()
{
  try
  {
    hardCamera->captureFrame(this->currentFrame);
  }
  catch(/*runtime_error error*/...)
  {
  //   std::cout << error.what() << std::endl;
    return;
  }

//   this->frameCounter++;
//   if(this->frameCounter >= 30)
//   {
//     this->frameRate = float( this->frameCounter/timer->ElapsedTime() );
//     quadros = 0;
//     timer->Start();
//   }

}

void
AbstractCamera::localize(silver::Position &position) const
{
  this->homography->transform2word(position);
}
