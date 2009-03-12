/**************************************************************************************
 Esta classe implementa o extrator de blobs. Ela recebe como entrada uma
IplImage em formato GREY (uma camada e 8 bits/pixel) e retorna um vector de
CvConnectedComp. Cada item desse vetor corresponde a um grupo de pixels com uma
mesma tonalidade, e cuja área é maior que uma área mínima preestabelecida.
**************************************************************************************/
#ifndef COLOR_BLOB_EXTRACTOR_HPP
#define COLOR_BLOB_EXTRACTOR_HPP

#include <opencv/cv.h>
#include <vector>
#include "silverTypes.hpp"
#include "scene.hpp"
#include <string>
#include <fstream>

class ColorBlobExtractor
{
public:

  ColorBlobExtractor(const std::vector<scene::Target> &vecTargets,
                     int areaThreshold=100);

  void extract(IplImage *imgEntrada,
               std::vector<silver::Blob> &vectorBlob);

  void init();

private:
  void FiltrarYUVImagem(IplImage *imagemYUV,IplImage *imagemRetorno);

  int areaThreshold; // Área mínima para que um blob seja reportado.

  unsigned char yuvLUT[256][256][256];

  // Número de cores diferentes que estão sendo representadas.
  int numCores;

  void CarregarYUVLUT(std::ifstream *streamYUV);

};

#endif
