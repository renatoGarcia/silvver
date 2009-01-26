#include <iostream>
#include <opencv/highgui.h>

#include "blobExtractor.hpp"
#include <cstring>

ColorBlobExtractor::ColorBlobExtractor(const std::vector<scene::Target> &vecTargets,
                                       int areaThreshold)
  :areaThreshold(areaThreshold)
  ,numCores(1)
{
  memset(this->yuvLUT, '\0', 256*256*256);
  cvNamedWindow("janela");
}

void
ColorBlobExtractor::extract(IplImage *imgEntrada,
                            std::vector<silver::Blob> &vectorBlob)
{
  IplImage *img = cvCreateImage(cvSize(640,480),
                                    IPL_DEPTH_8U, 3);

  cvCvtColor(imgEntrada, img, CV_BGR2RGB);

  cvShowImage("janela", img);
  cvWaitKey(5);

//   vectorMarca.clear();
//   double cor;
//   int altura     = imgEntrada->height;
//   int largura    = imgEntrada->width;
//   uchar *imgPtr  = (uchar *) imgEntrada->imageData;

//   CvPoint  inicioBlob;
//   CvScalar preto = cvScalar(0);  // Nova cor para substituir os blobs extraídos
//   CvConnectedComp ultimoBlob;

//   int pixelAtual  = 0,
//     ultimoPixel = altura * largura;
//   for(; pixelAtual<ultimoPixel ; pixelAtual++,imgPtr++)
//   {
//     if (*imgPtr==0)  continue; // Se não há nenhuma blob vai para o próximo pixel
//     cor = (double)*imgPtr;
//     inicioBlob = cvPoint(pixelAtual%largura,pixelAtual/largura);

//     cvFloodFill(imgEntrada,
// 		inicioBlob,
// 		preto,
// 		cvScalarAll(0),
// 		cvScalarAll(0),
// 		&ultimoBlob,
// 		CV_FLOODFILL_FIXED_RANGE,
// 		NULL);
//     ultimoBlob.value.val[0] = cor;//Coloca o valor da tonalidade anterior do blob, e não preto
//     if (ultimoBlob.area >= areaMinima)
//     {
//       double blobX = ultimoBlob.rect.x + ultimoBlob.rect.width/2;
//       double blobY = ultimoBlob.rect.y + ultimoBlob.rect.height/2;
//       Marca marca(blobX,blobY,(int)ultimoBlob.value.val[0],(float)ultimoBlob.area);
//       vectorMarca.push_back(marca);
//     }
//   }
}

void
ColorBlobExtractor::init()
{
//   std::vector<std::string> vecYuvTxt;
//   std::ifstream streamCores("Cores.txt", std::ios::in);
//   std::string strCores;

//   while( streamCores.peek()!= EOF )
//   {
//     streamCores >> strCores;
//     if (strCores.at(0) == '#') continue;
//     vecYuvTxt.push_back(strCores);
//   }

//   std::vector<std::string>::iterator iteYUV,fimVecYuvTxt;  // Iteradores para percorrer o vetor
//   iteYUV       = vecYuvTxt.begin();              // com o nome dos arquivos de texto
//   fimVecYuvTxt = vecYuvTxt.end();

//   std::ifstream *streamYUV;

//   for(this->numCores=1; iteYUV!=fimVecYuvTxt; iteYUV++,this->numCores++)
//   {
//     streamYUV = new std::ifstream(iteYUV->c_str(), std::ios::in);
//     CarregarYUVLUT(streamYUV);
//     delete streamYUV;
//   }
}

// Carregar um arquivo codificado como o CDT_Mars original
void
ColorBlobExtractor::CarregarYUVLUT(std::ifstream *streamYUV)
{
  int Ymax,Ymin,Umax,Umin,Vmax,Vmin;
  int codigoCor= this->numCores;
  while( streamYUV->peek()!=EOF )
  {
    *streamYUV >> Ymin >> Ymax >> Vmin >> Vmax >> Umin >> Umax;
    for(int y=Ymin;y<=Ymax;y++)
      for(int u=Umin;u<=Umax;u++)
	for(int v=Vmin;v<=Vmax;v++)
	  yuvLUT[y][u][v] = codigoCor;
  }
}

void
ColorBlobExtractor::FiltrarYUVImagem(IplImage *imagemYUV,IplImage *imagemRetorno)
{
  uchar *Yptr, *Uptr, *Vptr;
  uchar *GREYptr = (uchar *)imagemRetorno->imageData;

  Yptr = (uchar *)imagemYUV->imageData;
  Uptr = Yptr+1;
  Vptr = Yptr+2;

  int i    = 0,
    iMax = (imagemRetorno->height) * (imagemRetorno->width);
  for(; i<iMax ; i++)
  {
    *GREYptr = yuvLUT[*Yptr][*Uptr][*Vptr];

    Yptr += 3;
    Uptr += 3;
    Vptr += 3;
    GREYptr++;
  }
  cvDilate( imagemRetorno, imagemRetorno, NULL, 3 );
  cvErode( imagemRetorno, imagemRetorno, NULL, 3 );
}
