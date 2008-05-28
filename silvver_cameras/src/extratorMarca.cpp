#include "extratorMarca.hpp"
#include <opencv/highgui.h>
#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>

ExtratorMarca::ExtratorMarca(int largura, int altura)
{
  this->tracker.reset( new ARToolKitPlus::
                       TrackerSingleMarkerImpl<16,16,64,50,50>(640,480) );
}

int ExtratorMarca::Iniciar()
{
  if(!this->tracker->init("./data/no_distortion.cal",1000.0f, 7000.0f))
  {
    std::cerr << "ERROR: init() failed\n" << std::endl;
    return 1;
  }

  tracker->setLogger(&this->logger);
  tracker->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_BGR);

  // Define o tamanho da marca, em milímetros.
  tracker->setPatternWidth(150);

  // Define a porcentagem da borda no tamanho da imagem.
  tracker->setBorderWidth(/*0.250f*/0.125f);

  // Define um limiar entre as tonalidades de cor.
  tracker->setThreshold(100);

  tracker->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);

//   tracker[0]->setUndistortionMode(ARToolKitPlus::UNDIST_LUT);

  // switch to simple ID based markers
  // use the tool in tools/IdPatGen to generate markers
  tracker->setMarkerMode(ARToolKitPlus::MARKER_TEMPLATE);

  tracker->setImageProcessingMode(ARToolKitPlus::IMAGE_FULL_RES);

  tracker->addPattern("data/4x4patt/4x4_1.patt");
  tracker->addPattern("data/4x4patt/4x4_2.patt");
  tracker->addPattern("data/4x4patt/4x4_3.patt");
  tracker->addPattern("data/4x4patt/4x4_4.patt");
  tracker->addPattern("data/4x4patt/4x4_5.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_6.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_7.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_8.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_9.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_10.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_11.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_12.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_13.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_14.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_15.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_16.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_17.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_18.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_19.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_20.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_21.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_22.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_23.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_24.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_25.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_26.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_27.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_28.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_29.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_30.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_31.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_32.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_33.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_34.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_35.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_36.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_37.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_38.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_39.patt");
//     tracker[numLogger]->addPattern("data/4x4patt/4x4_40.patt");

    return 0;
}

void
ExtratorMarca::ExtrairMarcas(IplImage *imgEntrada,
                             std::vector<MarkerPontos> &vecMarkerPontos,
                             int logNum,int serial)
{
  vecMarkerPontos.clear();

  int totalMarca=0;
  ARToolKitPlus::ARMarkerInfo *marker_info;

  if (tracker->arDetectMarker((ARToolKitPlus::ARUint8*)imgEntrada->imageData, 100, &marker_info, &totalMarca) < 0 )
  {
    std::cerr << "error in arDetectMarker" << std::endl;
    return ;
  }

  MarkerPontos mP;

  for(int marca=0; marca<totalMarca; marca++)
  {
    switch(marker_info[marca].dir)
    {
    case 0:
      mP.verticeRef = Posicao(marker_info[marca].vertex[1][0],marker_info[marca].vertex[1][1]);
      mP.verticeSec = Posicao(marker_info[marca].vertex[0][0],marker_info[marca].vertex[0][1]);
      break;
    case 1:
      mP.verticeRef = Posicao(marker_info[marca].vertex[0][0],marker_info[marca].vertex[0][1]);
      mP.verticeSec = Posicao(marker_info[marca].vertex[3][0],marker_info[marca].vertex[3][1]);
      break;
    case 2:
      mP.verticeRef = Posicao(marker_info[marca].vertex[3][0],marker_info[marca].vertex[3][1]);
      mP.verticeSec = Posicao(marker_info[marca].vertex[2][0],marker_info[marca].vertex[2][1]);
      break;
    case 3:
      mP.verticeRef = Posicao(marker_info[marca].vertex[2][0],marker_info[marca].vertex[2][1]);
      mP.verticeSec = Posicao(marker_info[marca].vertex[1][0],marker_info[marca].vertex[1][1]);
      break;
    }
    mP.centro.x    = marker_info[marca].pos[0];
    mP.centro.y    = marker_info[marca].pos[1];
    mP.centro.id   = marker_info[marca].id;
    mP.centro.peso = marker_info[marca].cf;

    // Retira as marcas cuja confiança for menor que 50%
    if(mP.centro.id>=0 && marker_info[marca].cf>0.5)
    {
      vecMarkerPontos.push_back(mP);
    }
  }
}
