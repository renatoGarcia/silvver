#include "camera.hpp"
#include <stdexcept>

Camera::Camera(const CameraConfig& camConfig,double tempoInicial)
{
  this->configuracao = camConfig;

  cc0 = camConfig.cc.at(0);
  cc1 = camConfig.cc.at(1);

  fc0 = camConfig.fc.at(0);
  fc1 = camConfig.fc.at(1);

  kc0 = camConfig.kc.at(0);
  kc1 = camConfig.kc.at(1);
  kc2 = camConfig.kc.at(2);
  kc3 = camConfig.kc.at(3);
  kc4 = camConfig.kc.at(4);

  H00 = camConfig.H.at(0);
  H01 = camConfig.H.at(1);
  H02 = camConfig.H.at(2);
  H10 = camConfig.H.at(3);
  H11 = camConfig.H.at(4);
  H12 = camConfig.H.at(5);
  H20 = camConfig.H.at(6);
  H21 = camConfig.H.at(7);
  H22 = camConfig.H.at(8);

  switch(this->configuracao.modeloFisico)
  {
#ifdef HAVE_PGRFLYCAPTURE_HEADERS
  case CameraConfig::PGR:
    hardCamera = new PGR(camConfig.frequencia,camConfig.diretorio.c_str(),tempoInicial);
    break;
#endif
#ifdef HAVE_LIBDC1394_DC1394_CONTROL_H
  case CameraConfig::DC1394:
    hardCamera = new DC1394(0,camConfig.serial, HardCamera::FR_30);
    break;
#endif
  case CameraConfig::PseudoCam:
    hardCamera    = new PseudoCamera(733,camConfig.frequencia,camConfig.diretorio.c_str());
    break;
  default:
    throw std::invalid_argument("HardCamera include headers non accessible");
  }

  imgCamera = cvCreateImage( cvSize(camConfig.resolucao[0],camConfig.resolucao[1]), IPL_DEPTH_8U, 3 );

  //timer = new Timer();
  taxaDeQuadros = 0;
  quadros = 0;
}

Camera::~Camera()
{
  cvReleaseImage(&imgCamera);
}

void Camera::Iniciar()
{
  hardCamera->initialize();
}

double Camera::CapturarImagem()
{
  try
  {
    hardCamera->captureFrame(imgCamera);
  }
  catch(/*runtime_error error*/...)
  {
  //   std::cout << error.what() << std::endl;
    return 0;
  }

  quadros++;
  /*if (quadros>=30)
    {
    taxaDeQuadros = float( quadros/timer->ElapsedTime() );
    quadros = 0;
    timer->Start();
    }*/

  return 0;
}

void Camera::Localizar(Posicao &posicao)
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
