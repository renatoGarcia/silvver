#include "abstractCamera.hpp"

#include <cstddef>

#include <boost/ref.hpp>

#include "hardCameraFactory.hpp"
#include "tsPrint.hpp"

AbstractCamera::AbstractCamera(const scene::Camera& cameraConfig,
                               const std::string& serverIP,
                               unsigned connectionPort,
                               silver::TargetType targetType)
  :currentFrame(NULL)
  ,serverConnection(new Connection(serverIP, connectionPort))
  ,stopping(false)
  ,targetType(targetType)
  ,uid(cameraConfig.uid)
  ,hardCamera(HardCameraFactory::createHardCamera(cameraConfig))
  ,frameCounter(0)
  ,frameRate(0)
{
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

  this->hardCamera->createIplImage(this->currentFrame);
}

AbstractCamera::~AbstractCamera()
{
  cvReleaseImage(&currentFrame);
}

void
AbstractCamera::connect()
{
  this->serverConnection->connect();

  // Envia a segunda mensagem ao recepcionista("OK"), informando que
  // a conexao foi criada
  char msgOK[3] = "OK";
  this->serverConnection->send(msgOK,sizeof(msgOK));

  // Envia a terceira mensagem ao recepcionista, informando o código
  // do dado que será transmitido
  int tipoDado = this->targetType;
  this->serverConnection->send(&tipoDado,sizeof(tipoDado));

  char msg[3];
  // Espera uma mensagem de confirmação
  this->serverConnection->receive(msg,sizeof(msg));

  {PRINT_LOCK;
    std::cout << "Conecção câmera: " << this->uid
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
