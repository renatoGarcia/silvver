#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fcntl.h>

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

#define INFINITE -1

class Connection
{
public:

  Connection(unsigned port);

  void initialize();

  // Envia mensagem: msg � a mensagem e tamanho � o tamanho (sizeof) da mensagem
  void send(void *msg, int length);

  void receive(void *msg, unsigned length, int waitTime=INFINITE);

  inline unsigned getPort()
  {
    return this->port;
  }

private:

  const unsigned port;

  // Identifica para comunica��o com o cliente.
  SOCKET connectionSocket;

  // Define as caracteristicas do Servidor
  sockaddr_in infConnection;

  // SenderAddr.sin_family conter� o endere�o
  sockaddr_in SenderAddr;

  // Tamanhos em bytes de sockaddr_in SenderAddr.
#ifdef HAVE_WINDOWS_SOCKETS
  int SenderAddrSize;
#endif
#ifdef HAVE_LINUX_SOCKETS
  socklen_t SenderAddrSize;
#endif

  struct timeval tv;

  fd_set fdSet;


};

#endif
