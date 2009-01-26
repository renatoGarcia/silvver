#ifndef ABSTRACT_CAMERA_HPP
#define ABSTRACT_CAMERA_HPP

#include <opencv/cv.h>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/thread.hpp>
#include "silverTypes.hpp"
#include "hardCamera.hpp"
#include "scene.hpp"
#include "connection.hpp"
#include "homography.hpp"

/// Abstract base class to all abstract cameras.
class AbstractCamera : boost::noncopyable
{
public:

  enum TargetType{
    COLOR_BLOB = 103,
    ARTP_MARK  = 104
  };

  void run();

  void stop();

  virtual void operator()()=0;

  boost::scoped_ptr<boost::thread> runThread;

//   int TaxaQuadros();

  virtual ~AbstractCamera();

protected:

  TargetType targetType;

  IplImage *currentFrame;

  /// Conexao com o servidor, usada para enviar as localizações
  boost::scoped_ptr<Connection> connection;

//   scene::Camera cameraConfig;

  std::string UID;

  /** When this variable become True, the loop in operator() method implemented
   *  by a derivate class must exit */
  bool stopping;

  AbstractCamera(const scene::Camera& cameraConfig,
                 const std::string& serverIP,
                 unsigned connectionPort);

  void connect();

  void startHardCamera();

  // Atualiza a imagem em "*imgCamera", e retorna o instante no tempo em que elas foram capturadas.
  // Este tempo é dado em milisegundos, e é calculado levando em conta o "tempoInicial".
  void updateFrame();

  // Converte a posição das coordenadas em pixels para as coordenadas do mundo
  void localize(silver::Position &position) const;

private:

  // Hardware que fará a captura das imagens.
  boost::shared_ptr<HardCamera> hardCamera;

  boost::scoped_ptr<const Homography> homography;

  // Número de imagens processadas, usado para calcular a taxa média
  // de quadros por segundo.
  unsigned frameCounter;
  float frameRate;
};

#endif
