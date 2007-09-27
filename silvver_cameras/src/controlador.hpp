#ifndef CONTROLADOR_HPP
#define CONTROLADOR_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include "camera.hpp"
#include "cameraConfig.hpp"
#include "blobCamera.hpp"
#include "marcoCamera.hpp"
#include "conexao.hpp"
#include "silvver_tipos.hpp"
#include "timer.hpp"

using namespace std;
using namespace boost;
using namespace silvver;

/** Inicia as câmeras, conecta-as ao silvver-servidor, e agrupa as threads onde elas serão executadas.
 *
 */
class Controlador
{
private:

  /// Evita a criação e iniciação simultânea das câmeras.
  static mutex mutexInicarCamera;

  // Contém o caminho e os nomes dos arquivos de texto que definem as cores.
//   vector<string> vectorCores;

  /// Conjunto das estruturas de configuração de todas as câmeras.
  vector<CameraConfig> vecCameraConfig;

  /// Threads nas quais os métodos controladores das câmeras serão executados.
  vector<thread*> thCamera;

  /// Objeto da classe Conexao que se encontra que possui os dados para comunição com o recepcionista do silvver-servidor.
  Conexao *conexaoRecepcionista;

  // Porta � qual enviar a primeira mensagem, a fim de se
  // descobrir em qual porta a BlobCamera deve se conectar
  int portaReceptor;

  // Ip do receptor
  char *ipReceptor;

  // Ser� true quando for para encerrar as blobCameras
  static bool termina;

  // Controlar� uma blobCamera. � executado em uma thread separada.
  static void BlobCam(const CameraConfig &cameraConfig,vector<string> vecCores,int porta,char *ip,Conexao* conexao);

  // Controlar� uma marcaCamera. � executado em uma thread separada.
  static void MarcaCam(const CameraConfig &cameraConfig,int porta,char *ip,double tempoInicial,Conexao* conexao);

  // Prepara um conex�o para um nova blobC�mera no servidor
  void ConectarCamera(const CameraConfig &cameraConfig);

 public:

  Controlador(int porta,char *ip,vector<CameraConfig> &vecCameraConfig);

  ~Controlador();

  // Para cada par n�mero Serial/matriz H passados pelo arquivo
  // cameras.txt cria uma thread que rodar� o m�todo BlobCam()
  void RodarControlador();

  // Fecha todas as blobCameras, e prepara o controlador para terminar
  void PararControlador();
};

#endif
