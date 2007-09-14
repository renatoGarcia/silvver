#include "blobExtractor.hpp"

BlobExtractor::BlobExtractor(int minArea)
{
  this->areaMinima = minArea;
}

void BlobExtractor::ExtrairBlobs(IplImage *imgEntrada,vector<Marca> &vectorMarca)
{
  vectorMarca.clear();
  double cor;
  int altura     = imgEntrada->height;
  int largura    = imgEntrada->width;
  uchar *imgPtr  = (uchar *) imgEntrada->imageData;

  CvPoint  inicioBlob;
  CvScalar preto = cvScalar(0);  // Nova cor para substituir os blobs extraídos
  CvConnectedComp ultimoBlob;

  int pixelAtual  = 0,
    ultimoPixel = altura * largura;
  for(; pixelAtual<ultimoPixel ; pixelAtual++,imgPtr++)
  {
    if (*imgPtr==0)  continue; // Se não há nenhuma blob vai para o próximo pixel
    cor = (double)*imgPtr;
    inicioBlob = cvPoint(pixelAtual%largura,pixelAtual/largura);

    cvFloodFill(imgEntrada,
		inicioBlob,
		preto,
		cvScalarAll(0),
		cvScalarAll(0),
		&ultimoBlob,
		CV_FLOODFILL_FIXED_RANGE,
		NULL);
    ultimoBlob.value.val[0] = cor;//Coloca o valor da tonalidade anterior do blob, e não preto
    if (ultimoBlob.area >= areaMinima)
    {
      double blobX = ultimoBlob.rect.x + ultimoBlob.rect.width/2;
      double blobY = ultimoBlob.rect.y + ultimoBlob.rect.height/2;
      Marca marca(blobX,blobY,(int)ultimoBlob.value.val[0],(float)ultimoBlob.area);
      vectorMarca.push_back(marca);
    }
  }
}
