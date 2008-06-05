/** Cabeçalho interno da classe marco
 * @file   marco.hpp
 *
 * @brief  Este é o documento de cabeçalho utilizado internamente pelo silvver-cliente.
 *
 */

#ifndef MARCO_H
#define MARCO_H

#include "conexao.hpp"
#include "silverTypes.hpp"
#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <string>
#include <fstream>

using namespace silver;

/// Representa um alvo o qual será localizado pelo Silvver.
class Target
{
public:

  /** Construtor da classe marco aaa
   * @param ip Endereço IP da máquina onde está sendo executado o silvver-servidor.
   * @param id Identificador único da marca que será localizada.
   * @param registrar Caso seja atribuido um valor true os dados recebidos serão registrados em um arquivo texto.
   */
  Target(std::string ip, int id, bool log=false,
         unsigned receptionistPort=12000);

  /// Conecta o marco ao servidor.
  void connect();

  /// Desfaz a conexão do marco com o servidor.
  void disconnect();

  /** Retorna a última pose recebida do servidor.
   * @param x Coordenada x da pose, valor dado em metros.
   * @param y Coordenada y da pose, valor dado em metros.
   * @param teta Ângulo da pose, valor dado em radianos.
   */
  void getPose(double &x,double &y, double &theta);

  /** Retorna um objeto do tipo Pose representando a última pose recebida do servidor.
   */
  Pose getPose();

  /// Método que ficará continuamente esperando pelas mensagens do Silvver-servidor contendo a pose atual.
  /// Ele será executado em uma thread própria.
  void operator()();

private:

  /// Mutex para controlar a escrita e leitura na Pose ultimaPose.
  //  mutex mutexUltimaPose;
  boost::scoped_ptr<boost::mutex> mutexActualPose;

  /// Valor da última Pose do presente marco que foi localizada  pelas câmeras,
  /// e enviado através do Silvver-servidor.
  Pose actualPose;

  /// Número de identificação único do marco, que será usado para identificar o robô univocamente.
  const int ID_ROBOT;

  /// Endereço IP da máquina onde está sendo executado o Silvver-servidor.
  const std::string IP_SERVIDOR;

  /// Porta onde o recepcionista do servidor estará ligado.
  const unsigned RECEPTIONIST_PORT;

  /// Porta que será usada para a comunicação permanente com o Silvver-servidor.
  /// Seu valor será dado pelo mesmo quando requisitada a conexão.
//   unsigned int portaConexao;

  boost::scoped_ptr<Conexao> connection;

  /// Thread onde será executado o método ouvirServidor.
  boost::scoped_ptr<boost::thread> th;

  /// Envia um pedido de conexão ao recepcionista do Silvver-servidor, e a cria ao receber a resposta.
  void makeConnection();

  /// Sinalizador utilizado para terminar a função ouvirServidor.
  bool finalizarThread;

  boost::scoped_ptr<std::ofstream> arqRegistro;

};

#endif
