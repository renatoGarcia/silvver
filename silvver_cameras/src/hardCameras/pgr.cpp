#include "PGR.h"

#define MAX_CAMERAS 5

PGR::PGR(unsigned frequencia,char* diretorio,double tempoInicial)
{
  timer.iniciar(tempoInicial);

  this->numeroImagem = 0;

  this->diretorio = diretorio;

  if(frequencia < 12) this->frequencia=FLYCAPTURE_FRAMERATE_7_5;
  else if(frequencia < 20) this->frequencia=FLYCAPTURE_FRAMERATE_15;
  else this->frequencia=FLYCAPTURE_FRAMERATE_30;
}

PGR::~PGR()
{}

void PGR::Finalizar()
{
  flycaptureStop(contexto);
  flycaptureDestroyContext(contexto);
  for(int i=0;i<NUMERO_BUFFERS;i++)
  {
    delete this->buffers[i];
  }
}

void PGR::Iniciar(unsigned serial)
{
  for(int i=0; i<NUMERO_BUFFERS; i++)
  {
    this->buffers[i] = new unsigned char[ 640*480 ];
  }

  this->serial = serial;
  FlyCaptureError erro;

  FlyCaptureInfoEx flyInfo[MAX_CAMERAS];
  unsigned indiceCamera;
  unsigned totalCameras = MAX_CAMERAS;//Passa como argumento o tamanho do vetor flyInfo

  flycaptureBusEnumerateCamerasEx(flyInfo,&totalCameras);
  cout << "NUM Cam: " << totalCameras;

  for(int i=0;i<totalCameras;i++)
  {
    if(flyInfo[i].SerialNumber == serial)
    {
      indiceCamera = i;
      break;
    }
  }
  if(i==totalCameras)
  {
    cout << "CAMERA NAO ENCONTRADA" << endl;
    return;
  }

  erro = flycaptureCreateContext( &contexto );
  if ( erro != FLYCAPTURE_OK) throw("Criacao do contexto");

  erro = flycaptureInitializePlus(contexto, indiceCamera, NUMERO_BUFFERS, buffers);
  switch(erro)
  {
   case FLYCAPTURE_OK:
    break;
   case FLYCAPTURE_ALREADY_INITIALIZED:
    throw("Camera ja iniciada");
    break;
   default: //Algum outro erro qualquer
    throw("Iniciacao da camera");
  }

  if(flycaptureSetColorProcessingMethod( contexto,FLYCAPTURE_EDGE_SENSING)==FLYCAPTURE_OK)
  //{cout <<"Falhou"<<endl;}

  // Começa a capturar imagens
  erro = flycaptureStartLockNext(
           contexto,
           FLYCAPTURE_VIDEOMODE_ANY,
           this->frequencia );
  switch(erro)
  {
   case FLYCAPTURE_OK:
    break;
   case FLYCAPTURE_ALREADY_STARTED:
    throw("Camera ja esta capturando imagens");
    break;
   default: //Algum outro erro qualquer
    throw("Iniciacao da camera");
  }

  fcImagemBGR.pixelFormat = FLYCAPTURE_BGR;

  /* Cada char ocupa 8 bits, como o formato BGR ocupará 24 bits/pixel,     |
  |   temos que o tamanho necessário para o vetor uchar pData será:        |
  |   pixelsColunas x pixelsLinhas x 3                                    */
  fcImagemBGR.pData = new unsigned char[640 * 480 * 3];

  //memset( &fcImagemRaw, 0x0, sizeof( FlyCaptureImage ) );
}

void PGR::SalvarImagem()
{
  this->numeroImagem++;
  ostringstream nomeImagem;
  //cout << nomeImagem.str();
  //if (this->serial==5111119)
  //{
  nomeImagem << this->diretorio << this->numeroImagem << ".bmp";
  /*}
  else
  {
    nomeImagem << "E:/renato/cam32/" << this->numeroImagem << ".bmp";
  }*/

  FlyCaptureImage image,imageConverted;
  memset( &image, 0x0, sizeof( FlyCaptureImage ) );

  flycaptureGrabImage2( contexto, &image );

  imageConverted.pData = new unsigned char[ image.iCols * image.iRows * 3 ];
  imageConverted.pixelFormat = FLYCAPTURE_BGR;
  flycaptureConvertImage( contexto, &image, &imageConverted );
  flycaptureSaveImage(
    contexto,
    &imageConverted,
    nomeImagem.str().c_str(),
    FLYCAPTURE_FILEFORMAT_BMP );
}

double PGR::CapturarImg(IplImage *iplImgRetorno)
{
  FlyCaptureError erro;

  double instante = timer.tempoAbsoluto();
  erro = flycaptureLockNext( contexto, &fcImagemRaw );
  if ( erro != FLYCAPTURE_OK)
  {
    cerr << "ErroCap: " << flycaptureErrorToString(erro) << endl;
    return 0;
  }

  erro = flycaptureConvertImage( contexto, &(fcImagemRaw.image), &fcImagemBGR );
  if ( erro != FLYCAPTURE_OK)
    return 0;

  erro = flycaptureUnlock(contexto,fcImagemRaw.uiBufferIndex);

  // Limpa a IplImage recebida para o retorno
  cvReleaseImage( &iplImgRetorno );

  /* Cria um cabeçalho para a IplImage para receber a imagem de            |
  |   fcImagemBGR.pData, o mesmo tamanho de fcImagemBGR, 8 bits por        |
  |   pixel em cada camada, e três camadas                                */
  iplImgRetorno = cvCreateImageHeader( cvSize(fcImagemBGR.iCols,fcImagemBGR.iRows),
                                       IPL_DEPTH_8U,
                                       3
                                      );

  /* Associa a imagem com o cabeçalho criado  */
  cvSetData( iplImgRetorno, fcImagemBGR.pData, fcImagemBGR.iRowInc );

  /* Converte a imagem associada, em BGR para o formato YUV   */
  //cvCvtColor( iplImgRetorno, iplImgRetorno, CV_BGR2YCrCb );

  return instante;
}
