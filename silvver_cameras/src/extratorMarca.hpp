#ifndef EXTRATORMARCA_HPP
#define EXTRATORMARCA_HPP

#include <ARToolKitPlus/TrackerSingleMarker.h>
#include <iostream>
#include <opencv/cv.h>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include "silvver_tipos.hpp"

using namespace silvver;

// Estrutura para manter os pontos de interece de uma marca encontrada
struct MarkerPontos
{
  Posicao verticeRef;
  Posicao verticeSec;
  Marca centro;
};

class MyLogger : public ARToolKitPlus::Logger
{
    void artLog(const char* nStr)
    {
      std::clog << nStr << std::endl;
    }
};

class ExtratorMarca
{

private:

  MyLogger logger;

  boost::scoped_ptr<ARToolKitPlus::TrackerSingleMarker> tracker;

public:

  ExtratorMarca(int largura,int altura);

  int Iniciar();

  void ExtrairMarcas(IplImage *imgEntrada,std::vector<MarkerPontos> &vecMarkerPontos,int logNum,int serial);
};

#endif
