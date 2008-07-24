#include "blobCamera.hpp"
#include <vector>

BlobCamera::BlobCamera(CameraConfig cameraConfig, std::string serverIP,
                       unsigned connectionPort)
  :AbstractCamera(camConfig, serverIP, connectionPort)
{
  this->filtro.reset(new Filtro());
  this->blobExtractor.reset.(new BlobExtractor());
}

BlobCamera::~BlobCamera()
{}

int BlobCamera::Iniciar()
{
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

void
BlobCamera::operator()()
{
  this->connect();

  this->startHardCamera();

  this->filtro->Iniciar();

  std::vector<silver::Blob> vecBlob;
  silver::Package<silver::Blob> package;
  while(!stopping)
  {
    vecBlob.clear();
    this->updateFrame();

    this->blobExtractor->
  }


  //--------- VELHO
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
