#include "connection.hpp"

Connection::Connection(unsigned port)
  :port(port)
{
  this->SenderAddrSize = sizeof(SenderAddr);
}

void
Connection::initialize()
{
#ifdef HAVE_WINDOWS_SOCKETS
  WSADATA initialisation_win32;

  int erro = WSAStartup(MAKEWORD(2,2),&initialisation_win32);
  if(erro)
  {
//     printf("Suporte a sockets indisponivel.\n");
    return;
  }
#endif

  this->connectionSocket = socket(AF_INET,SOCK_DGRAM,0);
  if(this->connectionSocket<=0)
  {
//     printf("Impossível de Criar Socket.\n");
    return;
  }

  this->infConnection.sin_family = AF_INET;
  this->infConnection.sin_addr.s_addr = INADDR_ANY;
  this->infConnection.sin_port = htons(this->port);

  if(bind(this->connectionSocket,
          (struct sockaddr*)&infConnection,
          sizeof(infConnection))
     == -1 )
  {
//     perror("bind");
    return;
  }

  this->SenderAddr.sin_family = 0;
}

void
Connection::receive(void *msg, unsigned length, int waitTime)
{
  this->tv.tv_sec = waitTime;
  FD_ZERO(&this->fdSet);
  FD_SET(this->connectionSocket, &this->fdSet);
  if(waitTime != INFINITE)
  {
    select(this->connectionSocket+1, &this->fdSet, NULL, NULL, &this->tv);
  }
  else
  {
    select(this->connectionSocket+1, NULL, NULL, NULL, &this->tv);
  }

  if(FD_ISSET(this->connectionSocket , &this->fdSet))
  {
    int bytes_recebidos;

    bytes_recebidos = recvfrom(this->connectionSocket,
                               (char*) msg,
                               length,
                               0,
                               (struct sockaddr*)&SenderAddr,
                               &SenderAddrSize);

    //   if ( bytes_recebidos == -1 )
    //     printf("Erro d: A mensagem nao foi recebida corretamente.\n"/*,
    //                                                                   WSAGetLastError()*/ );
    //   if ( bytes_recebidos != length )
    //     printf("O length do dado recebido (%d) nao corresponde com o"
    //            " esperado (%d).\n", bytes_recebidos, length);
  }
}

void
Connection::send(void *msg, int length)
{
  int bytes_enviados;

  bytes_enviados = sendto(connectionSocket,(char*) msg, length,
                          0,(struct sockaddr*)&SenderAddr,
                          sizeof(SenderAddr));

//   if ( bytes_enviados == -1 )
// //     printf("Impossivel enviar dados\n");
//   if ( bytes_enviados != length )
//     printf("Dados nao foram enviados corretamente\n");

  return;
}
