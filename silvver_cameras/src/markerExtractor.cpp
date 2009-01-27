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

  int totalMarca=0;
  ARToolKitPlus::ARMarkerInfo *marker_info;

  if (tracker->arDetectMarker((ARToolKitPlus::ARUint8*)imgEntrada->imageData,
                              100,
                              &marker_info,
                              &totalMarca)
      < 0 )
  {
    throw detect_marker_error("Error in arDetectMarker method.");
  }

  MarkerPoints mP;

  for(int marca=0; marca<totalMarca; marca++)
  {
    switch(marker_info[marca].dir)
    {
    case 0:
      mP.verticeRef = silver::Position(marker_info[marca].vertex[1][0],
                                       marker_info[marca].vertex[1][1]);
      mP.verticeSec = silver::Position(marker_info[marca].vertex[0][0],
                                       marker_info[marca].vertex[0][1]);
      break;
    case 1:
      mP.verticeRef = silver::Position(marker_info[marca].vertex[0][0],
                                       marker_info[marca].vertex[0][1]);
      mP.verticeSec = silver::Position(marker_info[marca].vertex[3][0],
                                       marker_info[marca].vertex[3][1]);
      break;
    case 2:
      mP.verticeRef = silver::Position(marker_info[marca].vertex[3][0],
                                       marker_info[marca].vertex[3][1]);
      mP.verticeSec = silver::Position(marker_info[marca].vertex[2][0],
                                       marker_info[marca].vertex[2][1]);
      break;
    case 3:
      mP.verticeRef = silver::Position(marker_info[marca].vertex[2][0],
                                       marker_info[marca].vertex[2][1]);
      mP.verticeSec = silver::Position(marker_info[marca].vertex[1][0],
                                       marker_info[marca].vertex[1][1]);
      break;
    default:
      throw detect_marker_error("Invalid marker direction.");
      break;
    }
    mP.centro.x     = marker_info[marca].pos[0];
    mP.centro.y     = marker_info[marca].pos[1];
    mP.centro.id    = this->idMap.at(marker_info[marca].id);
    mP.centro.weigh = marker_info[marca].cf;

    // Drop the marks which confidence is lower than 50%
    if (mP.centro.id>=0 && marker_info[marca].cf>0.5)
    {
      vecMarkerPoints.push_back(mP);
    }
  }
}
