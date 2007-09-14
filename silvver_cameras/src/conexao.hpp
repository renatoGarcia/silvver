/**************************************************************************************
    Efetua a conexão do cliente
**************************************************************************************/
#ifndef CONEXAO_H
#define CONEXAO_H
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

class Conexao
{
private:

  // Socket para comunicação com o servidor.
  SOCKET SocketConexao;

  // Define as caracteristicas do cliente
  sockaddr_in infConexao;

  // SenderAddr.sin_family conterá o endereço
  sockaddr_in SenderAddr;

  // Tamanhos em bytes de sockaddr_in SenderAddr.
  #ifdef HAVE_WINDOWS_SOCKETS
  int SenderAddrSize;
  #endif
  #ifdef HAVE_LINUX_SOCKETS
  socklen_t SenderAddrSize;
  #endif

public:

  Conexao();

  int Iniciar(int porta,const char *ip);

  int Enviar(void *msg, int tamanho)const;

  int Receber(char *msg, int tamanho);
};

#endif
