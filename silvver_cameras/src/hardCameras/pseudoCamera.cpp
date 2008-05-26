#include "pseudoCamera.hpp"

extern boost::mutex mutexCout;

PseudoCamera::PseudoCamera(int totalImagens,unsigned frequencia,
                           const char* diretorio)
  :HardCamera(HardCamera::RESOLUTION_640x480, HardCamera::FR_30)
{
  this->endImagem = diretorio;
  this->imagemAtual = 1;
  this->totalImagens = 1000;//totalImagens;
  this->atraso = (int)((1.0/(double)frequencia)*1.0e9);
}

PseudoCamera::~PseudoCamera()
{}

void PseudoCamera::initialize()
{}

void PseudoCamera::saveFrame()
{}

void PseudoCamera::captureFrame(IplImage *iplImage)
{
  boost::xtime xt;
  boost::xtime_get(&xt, boost::TIME_UTC);
  xt.nsec += this->atraso;
  boost::thread::sleep(xt);

  // Limpa a IplImage recebida para o retorno
  cvReleaseImage( &iplImage );

  std::ostringstream s1;

  if(this->imagemAtual > this->totalImagens )
  {
    boost::mutex::scoped_lock lock(mutexCout);
    std::cout << "A pseudoCamera terminou" << std::endl;
    return;
  }

  //--------------------------------------------------------
  s1 << this->endImagem << this->imagemAtual << ".bmp";
  std::cout << "---------------------" <<std::endl << std::endl
            << s1.str() << std::endl;
  //--------------------------------------------------------
  iplImage = cvLoadImage( s1.str().c_str(), CV_LOAD_IMAGE_COLOR );

  this->imagemAtual++;

}

