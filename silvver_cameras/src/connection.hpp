#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_WINDOWS_SOCKETS
#include <winsock.h>
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

class Connection
{
public:

  Connection(std::string pairIP, unsigned port);

  void connect();

  void send(void *msg, int tamanho)const;

  void receive(char *msg, int tamanho);

  std::string getPairIP() const
  {
    return this->pairIP;
  }

  unsigned getPort() const
  {
    return this->port;
  }

private:

  const std::string pairIP;

  const unsigned port;

  // Socket para comunicação com o servidor.
  SOCKET SocketConnection;

  // Define as caracteristicas do cliente
  sockaddr_in infConnection;

  // SenderAddr.sin_family conterá o endereço
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
