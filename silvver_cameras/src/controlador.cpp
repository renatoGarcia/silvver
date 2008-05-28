#include "controlador.hpp"

// Mutex usado para escrever na saída padrão. Declarado em main.cpp.
extern boost::mutex mutexCout;

boost::mutex Controlador::mutexIniciarCamera;

void Controlador::conectar()
{
  this->conexao.Iniciar(this->porta,this->ipServidor.c_str());

  // Envia a segunda mensagem ao recepcionista("OK"), informando que
  // a conexao foi criada
  char msgOK[3] = "OK";
  this->conexao.Enviar( msgOK,sizeof(msgOK) );

  // Envia a terceira mensagem ao recepcionista, informando o código
  // do dado que será transmitido
  int tipoDado = cameraConfig.modeloAbstrato;
  this->conexao.Enviar( &tipoDado,sizeof(tipoDado) );

  char msg[3];
  // Espera uma mensagem de confirmação
  this->conexao.Receber( msg,sizeof(msg) );

  {boost::mutex::scoped_lock lock(mutexCout);
   std::cout << "CAMERA: " << cameraConfig.serial << " " << msg << std::endl;}
}

void Controlador::terminar()
{
  this->termina = true;
}

// --------------------- MarcoCameraControlador ----------------------------//

MarcoCameraControlador::MarcoCameraControlador(CameraConfig cameraConfig,
					       unsigned porta,
					       std::string ipServidor)
{
  this->cameraConfig = cameraConfig;
  this->porta = porta;
  this->ipServidor = ipServidor;
  this->termina = false;
}

void MarcoCameraControlador::operator()()
{
  {boost::mutex::scoped_lock lock(mutexIniciarCamera);
  //TODO: Implementar o controle de tempo
  double tempoInicial = 0;
  marcoCam.reset( new MarcoCamera(cameraConfig,tempoInicial) );
  marcoCamID = marcoCam->Iniciar();}

  this->conectar();

  std::vector<Ente> vecEnte;
  Pacote<Ente> pacote(marcoCamID);

  while(!termina)
  {
    marcoCam->ProcessarImagem(vecEnte);
    pacote.Empacotar(vecEnte);
    conexao.Enviar( &pacote,sizeof(pacote) );
  }
}
