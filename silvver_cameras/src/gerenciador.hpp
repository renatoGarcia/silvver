#ifndef GERENCIADOR_HPP
#define GERENCIADOR_HPP

#include <vector>
#include <string>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include "abstractCamera.hpp"
#include "markerCamera.hpp"
#include "cameraConfig.hpp"
#include "connection.hpp"
#include "timer.hpp"

class Gerenciador
{
private:
  /// Conjunto das estruturas de configuração de todas as câmeras.
  std::vector<CameraConfig> vecCameraConfig;

  /// Threads nas quais os métodos controladores das câmeras serão executados.
  std::vector< boost::shared_ptr< boost::thread > > thCamera;

  /// Objeto da classe Conexao que se encontra que possui os dados para comunição com o recepcionista do silvver-servidor.
  boost::scoped_ptr<Connection> conexaoRecepcionista;

  // Porta para qual enviar a primeira mensagem, a fim de se
  // descobrir em qual porta a BlobCamera deve se conectar
  int portaRecepcionista;

  // Endereço ip do servidor
  std::string ipServidor;

  /// Timer que sincronizado com tempo do servidor
  boost::scoped_ptr<Timer> timer;

  // Inicia a câmera abstrata descrita por cameraConfig
  void ConectarCamera(const CameraConfig &cameraConfig);

  std::vector< boost::shared_ptr<AbstractCamera> > vecAbstractCamera;

 public:

  Gerenciador(int portaRecepcionista, std::string ipServidor,
	      std::vector<CameraConfig> &vecCameraConfig);

  /// Inicia todas as câmeras abstradas contidas no vetod vecCameraConfig.
  void RodarGerenciador();

  /// Fecha todas as câmeras abstratas, e prepara o gerenciador para terminar.
  void PararGerenciador();
};

#endif
