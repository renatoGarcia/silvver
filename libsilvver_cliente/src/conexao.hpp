#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <unistd.h>

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

#include <string>

/// Implementa a conecção e encapsula a comunicação com o Silvver-servidor.
class Connection
{
public:

  Connection(std::string pairIP, unsigned port);

  ~Connection();

  typedef enum excecoes{ exc_tempoReceber } Excecoes;

  /** Inicia o socket SocketConnection. Utiliza o protocolo UDP/IP, e o associa à porta dada como
   * como parâmetro.
   * @return 1 em caso de erro, e 0 caso contrário.
   */
//   int Iniciar(int porta,const char *ip);
  void connect();

  /** Envia uma mensagem para o Silvver-servidor.
   * @param tamanho Tamanho da mensagem em Bytes.
   * @return O número de Bytes efetivamente enviados.
   */
  void send(void *msg, int tamanho)const;

  /** Recebe uma mensagem de Silvver-servidor.
   * @param tamanho Tamanho esperado em Bytes da mensagem a ser recebida.
   * @return O número de Bytes da mensagem recebida.
   */
  void receive(char *msg, int tamanho) throw(Excecoes);

  inline unsigned getPort() const
  {
    return this->port;
  }

private:

  const std::string pairIP;

  const unsigned port;

  /// Socket para comunicação com o servidor.
  SOCKET SocketConnection;

  // Define as caracteristicas do cliente
  sockaddr_in infConnection;

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
};

#endif
