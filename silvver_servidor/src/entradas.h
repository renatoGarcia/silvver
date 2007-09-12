#ifndef ENTRADAS_H
#define ENTRADAS_H

#include <vector>
#include <string>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include "conexao.h"
#include "tipos.h"
#include "saidas.h"
#include "blobTratador.h"
#include "marcaTratador.h"

using namespace std;
using namespace boost;
using namespace verlab;

enum TipoDado
{
  CORES  = 103,
  MARCAS = 104
};

/** Gerencia o conjunto de clientes que fornecem dados de entrada ao silvver-servidor.
 * Para cada tipo de dado de entrada deverá haver um método para tratá-lo.
 * Atualmente todos os clientes que fornecem dados de entrada são câmeras, e os tipos de dados
 * aceitos são a pose de marcos geométricos e a posição de blobs de cor.
 */
class Entradas
{
private:

  static auto_ptr<Entradas> instanciaUnica;
  static mutex mutexInstanciacao;
  Entradas();

  /// Em cada thread deste vetor estará sendo executado um método que tratará dos dados recebidos por um cliente.
  vector<thread*> threadCliente;

  /** Tratará os dados recebidos de uma blobCamera.
   *
   * @param objeto Apontador para a instância da classe Entradas, um ponteiro para this.
   * @param conexao Objeto de Conexao que contém as informações para a comunicação com o cliente através da rede.
   */
  static void ClienteBlobCam(Entradas *objeto, Conexao *conexao);

  /** Tratará os dados recebidos de uma marcoCamera.
   *
   * @param objeto Apontador para a instância da classe Entradas, um ponteiro para this.
   * @param conexao Objeto de Conexao que contém as informações para a comunicação com o cliente através da rede.
   */
  static void ClienteMarcaCam(Entradas *objeto, Conexao *conexao);

  Saidas *saidas;

  string tipoDado2string(TipoDado td);

public:

  /// Esta é uma classe solitária, este método deve ser usado ao invés do construtor.
  static Entradas* Instanciar();

  /// Recebe um objeto da classe conexão onde se encontram informações sobre um novo cliente e inicia o tratamento dos dados que serão recebidos.
  void AdicionarEntrada(Conexao *conexao);
};

#endif
