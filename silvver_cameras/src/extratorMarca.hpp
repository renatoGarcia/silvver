#ifndef MARKER_EXTRACTOR_HPP
#define MARKER_EXTRACTOR_HPP

#include <ARToolKitPlus/TrackerSingleMarker.h>
#include <iostream>
#include <opencv/cv.h>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include "silverTypes.hpp"
#include "xmlParser.hpp"

#define MAX_TARGETS 40

// Estrutura para manter os pontos de interece de uma marca encontrada
struct MarkerPoints
{
  silver::Position verticeRef;
  silver::Position verticeSec;
  silver::Ente centro;
};

class MyLogger : public ARToolKitPlus::Logger
{
    void artLog(const char* nStr)
    {
      std::clog << nStr << std::endl;
    }
};

class MarkerExtractor
{
public:

  MarkerExtractor(int width, int height,
                  const std::vector<TargetConfig> &targets);

  int init();

  void extract(const IplImage* const imgEntrada,
               std::vector<MarkerPoints> &vecMarkerPoints);

private:

  MyLogger logger;

  boost::scoped_ptr<ARToolKitPlus::TrackerSingleMarker> tracker;

  std::vector<std::string> vecDescriptionFilePath;

  /// Translates the inner artkp id to silver id
  int idMap[MAX_TARGETS];

};

#endif
