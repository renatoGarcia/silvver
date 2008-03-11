#include <iostream>
/**************************************************************************************
 Esta classe implementa o extrator de blobs. Ela recebe como entrada uma
IplImage em formato GREY (uma camada e 8 bits/pixel) e retorna um vector de
CvConnectedComp. Cada item desse vetor corresponde a um grupo de pixels com uma
mesma tonalidade, e cuja área é maior que uma área mínima preestabelecida.
**************************************************************************************/
#ifndef BLOBEXTRACTOR_HPP
#define BLOBEXTRACTOR_HPP

#include <opencv/cv.h>
#include <vector>
#include "silvver_tipos.hpp"

using namespace std;
using namespace silvver;

class BlobExtractor
{
private:
    int areaMinima; // Área mínima para que um blob seja reportado.
public:
    BlobExtractor(int minArea=100);
    void ExtrairBlobs(IplImage *imgEntrada,vector<Marca> &vectorMarca);
};

#endif
