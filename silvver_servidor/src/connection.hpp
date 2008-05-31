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

class Connection
{
public:

  Connection(unsigned port);

  void initialize();

  // Envia mensagem: msg � a mensagem e tamanho � o tamanho (sizeof) da mensagem
  void send(void *msg, int tamanho);

  void receive(void *msg, int tamanho);

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

};

#endif
