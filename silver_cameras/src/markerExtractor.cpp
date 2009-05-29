#include "markerExtractor.hpp"

#include <boost/foreach.hpp>

#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>
#include <opencv/highgui.h>

MarkerExtractor::MarkerExtractor(int width, int height,
                                 const std::vector<scene::Target>& targets)
  :logger()
  ,tracker(new ARToolKitPlus::
           TrackerSingleMarkerImpl<16,16,64,50,50>(width,height))
{
  int targetNum = 0;
  BOOST_FOREACH(scene::Target targetConfig, targets)
  {
    this->vecDescriptionFilePath.push_back(targetConfig.targetDefineFile);
    this->idMap.at(targetNum) = targetConfig.uid;
    targetNum++;
  }
}

void
MarkerExtractor::initialize()
{
  if(!this->tracker->init("./data/no_distortion.cal",
                          1000.0f,
                          7000.0f,
                          &this->logger))
  {
    throw initialize_error(this->logger.message);
  }

  this->tracker->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_BGR);

  // Define o tamanho da marca, em milímetros.
  this->tracker->setPatternWidth(150);

  // Define a porcentagem da borda no tamanho da imagem.
  this->tracker->setBorderWidth(/*0.250f*/0.125f);

  // Define um limiar entre as tonalidades de cor.
  this->tracker->setThreshold(100);

  this->tracker->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);

//   tracker[0]->setUndistortionMode(ARToolKitPlus::UNDIST_LUT);

  // switch to simple ID based markers
  // use the tool in tools/IdPatGen to generate markers
  this->tracker->setMarkerMode(ARToolKitPlus::MARKER_TEMPLATE);

  this->tracker->setImageProcessingMode(ARToolKitPlus::IMAGE_FULL_RES);

  BOOST_FOREACH(std::string filePath, this->vecDescriptionFilePath)
  {
    this->tracker->addPattern(filePath.c_str());
  }

  return;
}

void
MarkerExtractor::extract(const IplImage* const imgEntrada,
                         std::vector<MarkerPoints>& vecMarkerPoints) const
{
  vecMarkerPoints.clear();

  int nMarkers=0;
  ARToolKitPlus::ARMarkerInfo *marker_info;

  if (tracker->arDetectMarker((ARToolKitPlus::ARUint8*)imgEntrada->imageData,
                              100,
                              &marker_info,
                              &nMarkers)
      < 0 )
  {
    throw detect_marker_error("Error in arDetectMarker method.");
  }

  MarkerPoints mP;

  for (int marker = 0; marker < nMarkers; marker++)
  {
    // If the marker was not identified or its confidence is lower than 50%
    if ((marker_info[marker].id < 0) || (marker_info[marker].cf<0.5))
    {
      continue;
    }

    switch(marker_info[marker].dir)
    {
    case 0:
      mP.primaryVertex   = silver::Position(marker_info[marker].vertex[1][0],
                                            marker_info[marker].vertex[1][1]);
      mP.secondaryVertex = silver::Position(marker_info[marker].vertex[0][0],
                                            marker_info[marker].vertex[0][1]);
      break;
    case 1:
      mP.primaryVertex   = silver::Position(marker_info[marker].vertex[0][0],
                                            marker_info[marker].vertex[0][1]);
      mP.secondaryVertex = silver::Position(marker_info[marker].vertex[3][0],
                                            marker_info[marker].vertex[3][1]);
      break;
    case 2:
      mP.primaryVertex   = silver::Position(marker_info[marker].vertex[3][0],
                                            marker_info[marker].vertex[3][1]);
      mP.secondaryVertex = silver::Position(marker_info[marker].vertex[2][0],
                                            marker_info[marker].vertex[2][1]);
      break;
    case 3:
      mP.primaryVertex   = silver::Position(marker_info[marker].vertex[2][0],
                                            marker_info[marker].vertex[2][1]);
      mP.secondaryVertex = silver::Position(marker_info[marker].vertex[1][0],
                                            marker_info[marker].vertex[1][1]);
      break;
    default:
      throw detect_marker_error("Invalid marker direction.");
      break;
    }
    mP.center.x     = marker_info[marker].pos[0];
    mP.center.y     = marker_info[marker].pos[1];
    mP.center.uid    = this->idMap.at(marker_info[marker].id);
//     mP.center.weigh = marker_info[marker].cf;

    vecMarkerPoints.push_back(mP);
  }
}
