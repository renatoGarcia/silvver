/** Cabeçalho interno da classe marco
 * @file   marco.h
 *
 * @brief  Este é o documento de cabeçalho utilizado internamente pelo silvver-cliente.
 *
 */

#ifndef MARCO_H
#define MARCO_H

#include "conexao.h"
#include <tipos.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <string>
#include <fstream>

using namespace verlab;
using namespace boost;
using namespace std;

/// Representa um marco o qual será localizado pelo Silvver.
class Marco
{
private:

  /// Mutex para controlar a escrita e leitura na Pose ultimaPose.
  mutex mutexUltimaPose;

  /// Valor da última Pose do presente marco que foi localizada  pelas câmeras,
  /// e enviado através do Silvver-servidor.
  Pose ultimaPose;

  /// Número de identificação único do marco, que será usado para identificar o robô univocamente.
  const int ID_ROBO;

  /// Endereço IP da máquina onde está sendo executado o Silvver-servidor.
  const string IP;

  /// Porta que será usada para a comunicação permanente com o Silvver-servidor.
  /// Seu valor será dado pelo mesmo quando requisitada a conexão.
  unsigned portaConexao;

  Conexao *conexao;

  /// Thread onde será executado o método ouvirServidor.
  thread *thOuvirServidor;

  /// Envia um pedido de conexão ao recepcionista do Silvver-servidor, e a cria ao receber a resposta.
  void criarConexao();

  /// Método que ficará continuamente esperando pelas mensagens do Silvver-servidor contendo a pose atual.
  /// Ele será executado em uma thread própria.
  static void ouvirServidor(Marco *objeto);

  /// Sinalizador utilizado para terminar a função ouvirServidor.
  bool finalizarThread;

  bool registrar;
  ofstream *arqRegistro;

public:

  /** Construtor da classe marco aaa
   * @param ip Endereço IP da máquina onde está sendo executado o silvver-servidor.
   * @param id Identificador único da marca que será localizada.
   * @param registrar Caso seja atribuido um valor true os dados recebidos serão registrados em um arquivo texto.
   */
  Marco(std::string ip, int id, bool registrar=false);

  ~Marco();

  /// Conecta uma instância da classe ao servidor.
  void conectar();

  /** Retorna a última pose recebida do servidor.
   * @param x Coordenada x da pose, valor dado em metros.
   * @param y Coordenada y da pose, valor dado em metros.
   * @param teta Ângulo da pose, valor dado em radianos.
   */
  void getPose(double &x,double &y, double &teta);

  /** Retorna um objeto do tipo Pose representando a última pose recebida do servidor.
   */
  Pose getPose();
};

#endif
