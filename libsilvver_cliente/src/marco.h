#ifndef MARCO_H
#define MARCO_H

#include "conexao.h"
#include <tipos.h>
#include <boost/thread/thread.hpp>
#include <string>
#include <fstream>

using namespace verlab;
using namespace boost;
using namespace std;

class Marco
{
private:

  // Última pose retornada pelas câmeras
  Pose ultimaPose;

  const int ID_ROBO;

  // IP de onde está sendo executado o servidor.
  const string IP;

  unsigned portaConexao;

  Conexao *conexao;

  // Thread para o método que ficará ouvindo a conexão com o servidor para receber os dados.
  thread *thOuvirServidor;

  // Instancia o atributo conexão, conectando-o ao sistema de visão.
  void criarConexao();

  static void ouvirServidor(Marco *objeto);

  bool registrar;
  ofstream *arqRegistro;

public:

  // string ip: endereço IP do computador onde o servidor está sendo executado.
  // int id: id do marco que se deseja obter os dados de localização.
  // bool registrar: true para criar um registro no disco dos dados recebidos.
  Marco(string ip, int id, bool registrar=false);

  ~Marco();

  // Conecta o robô ao sistema de visão por câmeras.
  void conectar();

  // Retorna a última pose enviada pelo sistema de câmeras. Unidades em metros.
  Pose getPose();
  void getPose(double &x,double &y, double &teta);
};

#endif
