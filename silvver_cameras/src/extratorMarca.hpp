#ifndef EXTRATORMARCA_HPP
#define EXTRATORMARCA_HPP

#include <ARToolKitPlus/TrackerSingleMarker.h>
#include <iostream>
#include <opencv/cv.h>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include "silverTypes.hpp"

using namespace silver;

// Estrutura para manter os pontos de interece de uma marca encontrada
struct MarkerPontos
{
  Position verticeRef;
  Position verticeSec;
  Blob centro;
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

  void ExtrairMarcas(IplImage *imgEntrada,
                     std::vector<MarkerPontos> &vecMarkerPontos);
};

#endif
