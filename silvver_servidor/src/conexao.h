#include <iostream>
/**************************************************************************************
    Efetua a conex�o do servidor
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

  // Identifica para comunica��o com o cliente.
  SOCKET SocketConexao;

  // Define as caracteristicas do Servidor
  sockaddr_in infConexao;

  // SenderAddr.sin_family conter� o endere�o
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

    int Iniciar(int porta);

    // Envia mensagem: msg � a mensagem e tamanho � o tamanho (sizeof) da mensagem
    int Enviar(void *msg, int tamanho);

    int Receber(void *msg, int tamanho);

};

#endif
