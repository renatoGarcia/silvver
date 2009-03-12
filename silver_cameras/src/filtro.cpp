#include "filtro.hpp"
#include <cstring>

Filtro::Filtro()
{
  memset(this->yuvLUT, '\0', 256*256*256);
  numCores = 1;
}

void Filtro::Iniciar()
{
  std::vector<std::string> vecYuvTxt;
  std::ifstream streamCores("Cores.txt", std::ios::in);
  std::string strCores;

  while( streamCores.peek()!= EOF )
  {
    streamCores >> strCores;
    if (strCores.at(0) == '#') continue;
    vecYuvTxt.push_back(strCores);
  }

  std::vector<std::string>::iterator iteYUV,fimVecYuvTxt;  // Iteradores para percorrer o vetor
  iteYUV       = vecYuvTxt.begin();              // com o nome dos arquivos de texto
  fimVecYuvTxt = vecYuvTxt.end();

  std::ifstream *streamYUV;

  for(this->numCores=1; iteYUV!=fimVecYuvTxt; iteYUV++,this->numCores++)
  {
    streamYUV = new std::ifstream(iteYUV->c_str(), std::ios::in);
    CarregarYUVLUT(streamYUV);
    delete streamYUV;
  }
}

// Carregar um arquivo codificado como o CDT_Mars original
void Filtro::CarregarYUVLUT(std::ifstream *streamYUV)
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

void Filtro::FiltrarYUVImagem(IplImage *imagemYUV,IplImage *imagemRetorno)
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
