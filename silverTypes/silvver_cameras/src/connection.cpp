#include "connection.hpp"
#include <iostream>

Connection::Connection(std::string pairIP, unsigned port)
  :pairIP(pairIP)
  ,port(port)
{
  this->SenderAddrSize = sizeof(SenderAddr);
}

void
Connection::connect()
{
  #ifdef HAVE_WINDOWS_SOCKETS
  int erro;

  WSADATA initialisation_win32;

    erro = WSAStartup(MAKEWORD(2,2),&initialisation_win32);
    if(erro)
    {
      cerr << "Suporte a sockets indisponivel" << endl;
      return;
    }
  #endif

  // Criação do Socket para conexao UDP/IP
  SocketConnection = socket(AF_INET,SOCK_DGRAM,0);
  if (SocketConnection<=0)
  {
    std::cerr << "Erro ao criar o socket" << std::endl;
    return;
  }

  // Iniciação do Cliente
  this->infConnection.sin_family = AF_INET;
  this->infConnection.sin_addr.s_addr = inet_addr(this->pairIP.c_str());
  this->infConnection.sin_port = htons(this->port);
//   SenderAddr=infConnection;

  return;
}

void
Connection::receive(char *msg, int tamanho)
{
  int bytes_recebidos;

  bytes_recebidos = recvfrom(SocketConnection, msg, tamanho,
                             0, (struct sockaddr*)&SenderAddr,
                             &SenderAddrSize);

  if ( bytes_recebidos == -1 )
    std::cerr << "A mensagem nao foi recebida corretamente" << std::endl;
  if ( bytes_recebidos != tamanho )
    std::cerr << "O tamanho do dado recebido (" << bytes_recebidos
              << ") nao corresponde com o esperado (" << tamanho << ")"
              << std::endl;

}

void
Connection::send(void *msg, int tamanho)const
{
  int bytes_enviados;

  bytes_enviados=sendto(SocketConnection,(char*) msg, tamanho,
                        0,(struct sockaddr*)&infConnection,
                        sizeof(infConnection)                 );

  if ( bytes_enviados == -1 )
    printf("Impossivel enviar dados\n");
  if ( bytes_enviados != tamanho )
    printf("Dados nao foram enviados corretamente\n");

}
