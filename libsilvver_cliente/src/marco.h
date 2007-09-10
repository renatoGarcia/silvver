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

/// Representa um marco que será localizado.
class Marco
{
private:

  // Mutex para a escrita e leitura em ultimaPose.
  mutex mutexUltimaPose;

  // Última pose retornada pelas câmeras.
  Pose ultimaPose;

  const int ID_ROBO;

  // IP de onde está sendo executado o servidor.
  const string IP;

  unsigned portaConexao;

  Conexao *conexao;

  // Thread para o método que ficará ouvindo a conexão com o servidor para receber os dados.
  thread *thOuvirServidor;

  // Instancia o atributo conexão, conectando-o ao sistema de visão.
  void criarConexao();		/**< Testando o criarCon */

  static void ouvirServidor(Marco *objeto);
  bool finalizarThread; // Sinaliza para função ouvirServidor terminar.

  bool registrar;
  ofstream *arqRegistro;

public:

  /** Construtor da classe marco aaa
   *
   * @param ip Endereço IP da máquina onde está sendo executado o silvver-servidor.
   * @param id Identificador único da marca que será localizada.
   * @param registrar Caso seja atribuido um valor true os dados recebidos serão registrados em um arquivo texto.
   */
  Marco(std::string ip, int id, bool registrar=false);

  ~Marco();

  /// Conecta uma instância da classe ao servidor.
  void conectar();

  /** Retorna a última pose recebida do servidor.
   *
   * @param x Coordenada x da pose, valor dado em metros.
   * @param y Coordenada y da pose, valor dado em metros.
   * @param teta Ângulo da pose, valor dado em radianos.
   */
  void getPose(double &x,double &y, double &teta);
  /** Retorna um objeto do tipo Pose representando a última pose recebida do servidor.
   *  Não é implementada na interface da biblioteca. silvver_cliente.h.
   */
  Pose getPose();
};

#endif
