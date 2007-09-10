#ifndef SILVER_CLIENTE_H
#define SILVER_CLIENTE_H

#include <string>

/** Interface da biblioteca
 * @file   silvver_cliente.h
 * @brief  Esta é a interface que deverá ser incluida nos programas que utilizarão a biblioteca do
 *         programa silvver-cliente.
 *
 */

/// Representa um marco que será localizado.
class Marco
{
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
};

#endif
