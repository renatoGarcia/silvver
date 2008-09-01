#ifndef ABSTRACT_CAMERA_HPP
#define ABSTRACT_CAMERA_HPP

#include <opencv/cv.h>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/thread.hpp>
#include "silverTypes.hpp"
#include "hardCamera.hpp"
#include "xmlParser.hpp"
#include "connection.hpp"

// Classe base para as câmeras abstratas, como blobCamera e marcoCamera
class AbstractCamera : boost::noncopyable
{
public:

  enum TargetType{
    COLOR_BLOB = 103,
    ARTP_MARK  = 104
  };

  void run();

  void stop();

  virtual void operator()() = 0;

  boost::scoped_ptr<boost::thread> runThread;

//   int TaxaQuadros();

  virtual ~AbstractCamera();

protected:

  TargetType targetType;

  IplImage *currentFrame;

  /// Conexao com o servidor, usada para enviar as localizações
  boost::scoped_ptr<Connection> connection;

  CameraConfig cameraConfig;

  unsigned long UID;

  /** When this variable become True, the loop in operator() method implemented
   *  by a derivate class must exit */
  bool stopping;

  AbstractCamera(CameraConfig cameraConfig, std::string serverIP,
                 unsigned connectionPort);

  void connect();

  void startHardCamera();

  // Atualiza a imagem em "*imgCamera", e retorna o instante no tempo em que elas foram capturadas.
  // Este tempo é dado em milisegundos, e é calculado levando em conta o "tempoInicial".
  void updateFrame();

  // Converte a posição das coordenadas em pixels para as coordenadas do mundo
  void localize(silver::Position &position);

private:

  //Parâmetros intrínsecos e extrínsecos da câmera física.
  double cc0, cc1;
  double fc0, fc1;
  double kc0, kc1, kc2, kc3, kc4;
  double H00, H01, H02,
         H10, H11, H12,
         H20, H21, H22;

  // Hardware que fará a captura das imagens.
  boost::shared_ptr<HardCamera> hardCamera;

  // Número de imagens processadas, usado para calcular a taxa média
  // de quadros por segundo.
  unsigned frameCounter;
  float frameRate;

};

#endif
