#include "blobCamera.hpp"

BlobCamera::BlobCamera(const CameraConfig& camConfig,double tempoInicial)
 :Camera(camConfig,tempoInicial)
{
  filtro        = new Filtro();
  blobExtractor = new BlobExtractor();
  imgTrabalho   = cvCreateImage( cvSize(camConfig.resolucao[0],camConfig.resolucao[1]), IPL_DEPTH_8U, 1 );
}

BlobCamera::~BlobCamera()
{
  cvReleaseImage(&imgTrabalho);
  delete filtro;
  delete blobExtractor;
}

int BlobCamera::Iniciar()
{
  filtro->Iniciar();
  try
  {
    Camera::Iniciar();
  }
  catch(std::string erro)
  {
    std::cout << "Erro: " << erro << std::endl;
    abort();
  }

  return configuracao.serial;
}

void BlobCamera::ProcessarImagem(std::vector<Marca> &vecMarca)
{
  double tempoAbsoluto;

  vecMarca.clear();
  tempoAbsoluto = Camera::CapturarImagem();

  filtro->FiltrarYUVImagem(Camera::imgCamera, imgTrabalho);
  blobExtractor->ExtrairBlobs(imgTrabalho, vecMarca);

  std::vector<Marca>::iterator iteVecMarca = vecMarca.begin();
  std::cout << vecMarca.size() << "  !!!" << std::endl;
  for(;iteVecMarca<vecMarca.end();iteVecMarca++)
  {
    Camera::Localizar(*iteVecMarca);
  }
}
