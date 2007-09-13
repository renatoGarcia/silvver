#include <iostream>
/**************************************************************************************
 Esta classe implementa o extrator de blobs. Ela recebe como entrada uma            
IplImage em formato GREY (uma camada e 8 bits/pixel) e retorna um vector de
CvConnectedComp. Cada item desse vetor corresponde a um grupo de pixels com uma
mesma tonalidade, e cuja �rea � maior que uma �rea m�nima preestabelecida.
**************************************************************************************/
#ifndef BLOBEXTRACTOR_H
#define BLOBEXTRACTOR_H

#include <cv.h>
#include <vector>
#include "tipos.h"

using namespace std;
using namespace verlab;

class BlobExtractor
{
private:
    int areaMinima; // �rea m�nima para que um blob seja reportado.
public:
    BlobExtractor(int minArea=100);
    void ExtrairBlobs(IplImage *imgEntrada,vector<Marca> &vectorMarca);
};

#endif