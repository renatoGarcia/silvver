#include <iostream>

#ifndef EXTRATORMARCA_HPP
#define EXTRATORMARCA_HPP

//#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>
//using namespace ARToolKitPlus;
#include <cv.h>
#include <vector>
#include "tipos.hpp"
#include <boost/thread/mutex.hpp>

using namespace std;
using namespace verlab;
using namespace boost;
// Estrutura para manter os pontos de interece de uma marca encontrada
struct MarkerPontos
{
  Posicao verticeRef;
  Posicao verticeSec;
  Marca centro;
};

class ExtratorMarca
{

private:

  //MyLogger *logger;

  //TrackerSingleMarker *tracker;

  static int numLogger;

public:

  ExtratorMarca(int largura,int altura);

  int Iniciar();

  void ExtrairMarcas(IplImage *imgEntrada,vector<MarkerPontos> &vecMarkerPontos,int logNum,int serial);
};

#endif
