#ifndef SILVER_CLIENTE_H
#define SILVER_CLIENTE_H

#include <string>

class Marco
{
public:

  // string ip: endereço IP do computador onde o servidor está sendo executado.
  // int id: id do marco que se deseja obter os dados de localização.
  // bool registrar: true para criar um registro no disco dos dados recebidos.
  Marco(std::string ip, int id, bool registrar=false);

  ~Marco();

  // Conecta o robô ao sistema de visão por câmeras.
  void conectar();

  // Retorna a última pose enviada pelo sistema de câmeras. Unidades em metros.
  void getPose(double &x,double &y, double &teta);
};

#endif
