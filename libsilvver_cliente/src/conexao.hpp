/** Cabeçalho da classe Conexao
 * @file   conexao.hpp
 *
 */

#ifndef CONEXAO_H
#define CONEXAO_H

#include <string>
#include <unistd.h> //TODO: Verificar compatibilidade com windows para o uso
                    //      de select()
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_WINDOWS_SOCKETS
#include <winsock2.h>
#include <stdio.h>
#endif

#ifdef HAVE_LINUX_SOCKETS
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SOCKET int
#endif

/// Implementa a conecção e encapsula a comunicação com o Silvver-servidor.
class Conexao
{
private:

  /// Socket para comunicação com o servidor.
  SOCKET SocketConexao;

  // Define as caracteristicas do cliente
  sockaddr_in infConexao;

  // SenderAddr.sin_family conterá o endereço
  sockaddr_in SenderAddr;

  fd_set master_readfds;
  struct timeval TV;

  // Tamanhos em bytes de sockaddr_in SenderAddr.
  #ifdef HAVE_WINDOWS_SOCKETS
  int SenderAddrSize;
  #endif
  #ifdef HAVE_LINUX_SOCKETS
  socklen_t SenderAddrSize;
  #endif

public:

  Conexao();

  ~Conexao();

  typedef enum excecoes{ exc_tempoReceber } Excecoes;

  /** Inicia o socket SocketConexao. Utiliza o protocolo UDP/IP, e o associa à porta dada como
   * como parâmetro.
   * @return 1 em caso de erro, e 0 caso contrário.
   */
  int Iniciar(int porta,const char *ip);

  /** Envia uma mensagem para o Silvver-servidor.
   * @param tamanho Tamanho da mensagem em Bytes.
   * @return O número de Bytes efetivamente enviados.
   */
  int Enviar(void *msg, int tamanho)const;

  /** Recebe uma mensagem de Silvver-servidor.
   * @param tamanho Tamanho esperado em Bytes da mensagem a ser recebida.
   * @return O número de Bytes da mensagem recebida.
   */
  int Receber(char *msg, int tamanho) throw(Excecoes);
};

#endif
