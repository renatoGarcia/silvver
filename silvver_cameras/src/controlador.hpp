#ifndef CONTROLADOR_HPP
#define CONTROLADOR_HPP

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <string>
#include "cameraConfig.hpp"
#include "camera.hpp"
#include "blobCamera.hpp"
#include "marcoCamera.hpp"
#include "conexao.hpp"
#include "silvver_tipos.hpp"


class Controlador : boost::noncopyable
{
protected:


  /// Classe que descreve as configurações da câmera abstrada
  CameraConfig cameraConfig;

  /// Número de identificação único da Câmera abstrata
  unsigned marcoCamID;


public:

};


class MarcoCameraControlador : public Controlador
{
private:

  /// Câmera abstrata para a localização de marcos
  boost::scoped_ptr<MarcoCamera> marcoCam;

public:

  MarcoCameraControlador(CameraConfig cameraConfig, unsigned Porta,
			 std::string ipServidor);

};


#endif
