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
  catch(string erro)
  {
    cout << "Erro: " << erro << endl;
    abort();
  }

  return configuracao.serial;
}

void BlobCamera::ProcessarImagem(vector<Marca> &vecMarca)
{
  double tempoAbsoluto;

  vecMarca.clear();
  tempoAbsoluto = Camera::CapturarImagem();

  filtro->FiltrarYUVImagem(Camera::imgCamera, imgTrabalho);
  blobExtractor->ExtrairBlobs(imgTrabalho, vecMarca);

  vector<Marca>::iterator iteVecMarca = vecMarca.begin();
  cout << vecMarca.size() << "  !!!" << endl;
  for(;iteVecMarca<vecMarca.end();iteVecMarca++)
  {
    Camera::Localizar(*iteVecMarca);
  }
}
