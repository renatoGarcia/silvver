#include "abstractCamera.hpp"
#include <stdexcept>
#include "hardCameras/modelos.hpp"

// Mutex usado para escrever na saída padrão. Declarado em main.cpp.
extern boost::mutex mutexCout;

boost::mutex AbstractCamera::mutexStartHardCamera;

AbstractCamera::AbstractCamera(CameraConfig cameraConfig, std::string serverIP,
                               unsigned connectionPort)
{
  this->cameraConfig = cameraConfig;

  this->cc0 = cameraConfig.cc.at(0);
  this->cc1 = cameraConfig.cc.at(1);

  this->fc0 = cameraConfig.fc.at(0);
  this->fc1 = cameraConfig.fc.at(1);

  this->kc0 = cameraConfig.kc.at(0);
  this->kc1 = cameraConfig.kc.at(1);
  this->kc2 = cameraConfig.kc.at(2);
  this->kc3 = cameraConfig.kc.at(3);
  this->kc4 = cameraConfig.kc.at(4);

  this->H00 = cameraConfig.H.at(0);
  this->H01 = cameraConfig.H.at(1);
  this->H02 = cameraConfig.H.at(2);
  this->H10 = cameraConfig.H.at(3);
  this->H11 = cameraConfig.H.at(4);
  this->H12 = cameraConfig.H.at(5);
  this->H20 = cameraConfig.H.at(6);
  this->H21 = cameraConfig.H.at(7);
  this->H22 = cameraConfig.H.at(8);

  this->frameCounter = 0;
  this->frameRate = 0;

  this->UID = cameraConfig.serial;

  this->connection.reset(new Connection(serverIP, connectionPort));

  this->stopping = false;

  HardCamera::Resolution resolution;
  if(cameraConfig.resolucao.at(0)==640 && cameraConfig.resolucao.at(1)==480)
  {
    resolution = HardCamera::RESOLUTION_640x480;
  }

  HardCamera::FrameRate frameRate;
  if(cameraConfig.frameRate == 7.5)
  {
    frameRate = HardCamera::FR_7_5;
  }
  else if(cameraConfig.frameRate == 15)
  {
    frameRate = HardCamera::FR_15;
  }
  else if(cameraConfig.frameRate == 30)
  {
    frameRate = HardCamera::FR_30;
  }
  else
  {
    throw std::invalid_argument("Frame Rate not allowed");
  }

  switch(this->cameraConfig.modeloFisico)
  {
#ifdef HAVE_PGRFLYCAPTURE_HEADERS
  case CameraConfig::PGR:
    this->hardCamera.reset(new PGR(cameraConfig.frameRate,
                                   cameraConfig.diretorio.c_str(),
                                   tempoInicial));
    break;
#endif
#ifdef HAVE_LIBDC1394_DC1394_CONTROL_H
  case CameraConfig::DC1394:
    this->hardCamera.reset(new DC1394(0,
                                      cameraConfig.serial,
                                      frameRate,
                                      resolution));
    break;
#endif
  case CameraConfig::PseudoCam:
    this->hardCamera.reset(new PseudoCamera(733,
                                            cameraConfig.frameRate,
                                            cameraConfig.diretorio.c_str()));
    break;
  default:
    throw std::invalid_argument("HardCamera include headers non accessible");
  }

  this->hardCamera->createIplImage(actualFrame);

}

AbstractCamera::~AbstractCamera()
{
  cvReleaseImage(&actualFrame);
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
  int tipoDado = cameraConfig.modeloAbstrato;
  this->connection->send(&tipoDado,sizeof(tipoDado));

  char msg[3];
  // Espera uma mensagem de confirmação
  this->connection->receive(msg,sizeof(msg));

  {boost::mutex::scoped_lock lock(mutexCout);
    std::cout << "CAMERA: " << cameraConfig.serial << " " << msg << std::endl;
  }
}

void
AbstractCamera::startHardCamera()
{
  boost::mutex::scoped_lock lock(mutexStartHardCamera);
  this->hardCamera->initialize();
}

void
AbstractCamera::stop()
{
  this->stopping = true;
}

void
AbstractCamera::updateFrame()
{
  try
  {
    hardCamera->captureFrame(actualFrame);
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
AbstractCamera::localize(Posicao &posicao)
{

  //------------------------------------- Parâmetros Intrínsecos
  double xc[2];
  xc[0] = posicao.x;
  xc[1] = posicao.y;
  double x[2];
  double x_distort[2];
  x_distort[0] = (xc[0] - cc0)/fc0;
  x_distort[1] = (xc[1] - cc1)/fc1;

  double delta_x[2];
  double k_radial, r2;
  x[0] = x_distort[0];
  x[1] = x_distort[1];
  for (int i = 0; i < 20; i++)
  {
    r2 = x[0]*x[0] + x[1]*x[1];
    k_radial = 1.0 + kc0 * r2 + kc1 * pow(r2,2) + kc4 * pow(r2,3);
    delta_x[0] = 2 * kc2 * x[0] * x[1] + kc3*(r2 + 2 * pow(x[0],2));
    delta_x[1] = kc2*(r2 + 2 * pow(x[1],2)) + 2 * kc3 * x[0] * x[1];
    x[0] = (x_distort[0] - delta_x[0])/k_radial;
    x[1] = (x_distort[1] - delta_x[1])/k_radial;
  }
  posicao.x = x[0]*fc0 + cc0;
  posicao.y = x[1]*fc1 + cc1;

  //------------------------------------- Parâmetros Extrínsecos

  double escala,xPixel,yPixel,xMundo,yMundo;

  xPixel = posicao.x;
  yPixel = posicao.y;

  escala = H20*xPixel + H21*yPixel + H22;
  xMundo = (H00*xPixel +
            H01*yPixel +
            H02) / escala;
  yMundo = (H10*xPixel +
            H11*yPixel +
            H12) / escala;

  posicao.x = xMundo;
  posicao.y = yMundo;

}
