#include "conexao.hpp"
#include <iostream>

Connection::Connection(std::string pairIP, unsigned port)
  :pairIP(pairIP)
  ,port(port)
{
  this->SenderAddrSize = sizeof(SenderAddr);
  TV.tv_sec = 0;
  TV.tv_usec = 100000;
}

Connection::~Connection()
{
  close(SocketConnection);
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
      std::cerr << "Suporte a sockets indisponivel" << std::endl;
      return;
    }
  #endif

  // Criação do Socket para conexao UDP/IP
  if( (SocketConnection = socket(AF_INET,SOCK_DGRAM,0)) <= 0 )
  {
    std::cerr << "Erro ao criar o socket" << std::endl;
    return;
  }

  // Iniciação do Cliente
  infConnection.sin_family = AF_INET;
  infConnection.sin_addr.s_addr = inet_addr(this->pairIP.c_str());
  infConnection.sin_port = htons(this->port);

  FD_ZERO(&master_readfds);
  FD_SET(SocketConnection, &master_readfds);

  return;
}

void
Connection::receive(char *msg, int tamanho) throw(Excecoes)
{
  int bytes_recebidos;
  fd_set readfds;
  struct timeval espera;

  readfds = master_readfds;
  espera = TV;

  select(SocketConnection+1, &readfds, NULL, NULL, &espera);
  if(FD_ISSET(SocketConnection, &readfds))
  {
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
  else
  {
    Excecoes ex = exc_tempoReceber;
    throw ex;
  }
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
