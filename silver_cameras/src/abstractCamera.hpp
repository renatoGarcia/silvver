#ifndef ABSTRACT_CAMERA_HPP
#define ABSTRACT_CAMERA_HPP

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

#include <opencv/cv.h>

#include "connection.hpp"
#include "hardCamera.hpp"
#include "homography.hpp"
#include "scene.hpp"
#include "silverTypes.hpp"

/// Abstract base class to all abstract cameras.
class AbstractCamera : boost::noncopyable
{
public:

  /// Start a new thread and run the operator() method.
  void run();

  /// Stop the operator() method and the thread.
  void stop();

  virtual ~AbstractCamera();

  // It's the method which will be called when a new thread was created, and
  // will make all camera work.
  virtual void operator()()=0;

protected:

  IplImage *currentFrame;

  /// Connection with the silver-server used to send the target localizations.
  const boost::shared_ptr<Connection> serverConnection;

  /** When this variable become True, the loop in operator() method implemented
   *  by a derivate class must exit */
  bool stopping;

  AbstractCamera(const scene::Camera& cameraConfig,
                 boost::shared_ptr<Connection> connection);

  // Atualiza a imagem em "*imgCamera", e retorna o instante no tempo em que elas foram capturadas.
  // Este tempo é dado em milisegundos, e é calculado levando em conta o "tempoInicial".
  void updateFrame();

  // Converte a posição das coordenadas em pixels para as coordenadas do mundo
  void localize(silver::Position &position) const;

private:

  boost::scoped_ptr<boost::thread> runThread;

  // Hardware que fará a captura das imagens.
  const boost::shared_ptr<HardCamera> hardCamera;

  boost::scoped_ptr<const Homography> homography;

  // Número de imagens processadas, usado para calcular a taxa média
  // de quadros por segundo.
  unsigned frameCounter;
  float frameRate;
};

#endif
