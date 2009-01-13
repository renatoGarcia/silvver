#include "abstractCamera.hpp"
#include "hardCameraFactory.hpp"
#include <boost/ref.hpp>
#include <boost/thread/mutex.hpp>
#include <fstream>

// Mutex usado para escrever na saída padrão. Declarado em main.cpp.
extern boost::mutex mutexCout;

AbstractCamera::AbstractCamera(const CameraConfig& cameraConfig,
                               const std::string& serverIP,
                               const unsigned connectionPort)
{
  this->useLut = cameraConfig.useLut;

  if (this->useLut)
  {
    std::ifstream lutXFile(cameraConfig.lut.at(0).c_str());
    std::ifstream lutYFile(cameraConfig.lut.at(1).c_str());
    this->width = cameraConfig.resolution.at(0);
    this->height = cameraConfig.resolution.at(1);

    lutX = new double*[this->height + 1];
    for (int i = 0; i < this->height + 1; i++)
      lutX[i] = new double[this->width + 1];

    lutY = new double*[this->height + 1];
    for (int i = 0; i < this->height + 1; i++)
      lutY[i] = new double[this->width + 1];

    for(int i = 0; i < this->height; i++)
      for (int j = 0; j < this->width; j++)
        lutXFile >> lutX[i][j];

    for(int i = 0; i < this->height; i++)
      for (int j = 0; j < this->width ; j++)
        lutYFile >> lutY[i][j];

    // Preenchendo a "borda extra"
    for(int i = 0; i < this->height; i++)
    {
      lutX[i][this->width] = lutX[i][this->width-1];
      lutY[i][this->width] = lutY[i][this->width-1];
    }
    for(int j = 0; j < this->width; j++)
    {
      lutX[this->height][j] = lutX[this->height-1][j];
      lutY[this->height][j] = lutY[this->height-1][j];
    }
  }

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

  this->UID = cameraConfig.uid;

  this->connection.reset(new Connection(serverIP, connectionPort));

  this->stopping = false;

  this->hardCamera = HardCameraFactory::createHardCamera(cameraConfig);

  this->hardCamera->createIplImage(currentFrame);
}

AbstractCamera::~AbstractCamera()
{
   if (useLut)
   {
      for (int i = 0; i < this->height; i++)
      {
	 delete lutX[i];
	 delete lutY[i];
      }
      delete lutX;
      delete lutY;
   }

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

  {boost::mutex::scoped_lock lock(mutexCout);
    std::cout << "Conecção câmera: " << cameraConfig.uid << " " << msg << std::endl;
  }
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
   if (this->useLut)
   {
      double x = position.x;
      double y = position.y;
      double xFactor = x - floor(x);
      double yFactor = y - floor(y);
      int iX = floor(x);
      int iY = floor(y);

      if (iY >=0 && iX >= 0 && iY < height && iX < width)
      {
	 position.x = lutX[iY][iX] * (1-xFactor)*(1-yFactor) +
	           lutX[iY + 1][iX] * (1-xFactor)*(yFactor) +
	           lutX[iY][iX + 1] * (xFactor)*(1-yFactor) +
	           lutX[iY + 1][iX + 1] * (xFactor)*(yFactor);

	 position.y = lutY[iY][iX] * (1-xFactor)*(1-yFactor) +
	           lutY[iY + 1][iX] * (1-xFactor)*(yFactor) +
	           lutY[iY][iX + 1] * (xFactor)*(1-yFactor) +
	           lutY[iY + 1][iX + 1] * (xFactor)*(yFactor);
      }
      else
      {
	 std::cerr << "Cuidado! Tentativa de acessar posicao invalida" << std::endl;
      }
   }
   else
   {
      //------------------------------------- Parâmetros Intrínsecos
      double xc[2];
      xc[0] = position.x;
      xc[1] = position.y;
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
      position.x = x[0]*fc0 + cc0;
      position.y = x[1]*fc1 + cc1;

      //------------------------------------- Parâmetros Extrínsecos

      double escala,xPixel,yPixel,xMundo,yMundo;

      xPixel = position.x;
      yPixel = position.y;

      escala = H20*xPixel + H21*yPixel + H22;
      xMundo = (H00*xPixel +
		H01*yPixel +
		H02) / escala;
      yMundo = (H10*xPixel +
		H11*yPixel +
		H12) / escala;

      position.x = xMundo;
      position.y = yMundo;
   }
}
