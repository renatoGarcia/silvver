#include "conexao.h"
#include <iostream>

using namespace std;

Conexao::Conexao()
{
  this->SenderAddrSize = sizeof(SenderAddr);
  TV.tv_sec = 0;
  TV.tv_usec = 100000;
}

int Conexao::Iniciar(int porta,const char *ip)
{
  int erro;

  #ifdef HAVE_WINDOWS_SOCKETS
    WSADATA initialisation_win32;

    erro = WSAStartup(MAKEWORD(2,2),&initialisation_win32);
    if(erro)
    {
      cerr << "Suporte a sockets indisponivel" << endl;
      return 1;
    }
  #endif

  // Criação do Socket para conexao UDP/IP
  if( (SocketConexao = socket(AF_INET,SOCK_DGRAM,0)) <= 0 )
  {
    cerr << "Erro ao criar o socket" << endl;
    return 1;
  }

  // Iniciação do Cliente
  infConexao.sin_family = AF_INET;            // Indica a utilização do IPv4
  infConexao.sin_addr.s_addr = inet_addr(ip); // Endereço IP do servidor
  infConexao.sin_port = htons(porta);         // Porta utiliazada

  FD_ZERO(&master_readfds);
  FD_SET(SocketConexao, &master_readfds);

  return 0;
}

int Conexao::Receber(char *msg, int tamanho) throw(Excecoes)
{
  int bytes_recebidos;
  fd_set readfds;
  struct timeval espera;

  readfds = master_readfds;
  espera = TV;

  select( SocketConexao+1, &readfds, NULL, NULL, &espera);
  if( FD_ISSET(SocketConexao, &readfds) )
  {

    bytes_recebidos = recvfrom(SocketConexao, msg, tamanho,
			       0, (struct sockaddr*)&SenderAddr,
			       &SenderAddrSize);

    if ( bytes_recebidos == -1 )
      cerr << "A mensagem nao foi recebida corretamente" << endl;
    if ( bytes_recebidos != tamanho )
      cerr << "O tamanho do dado recebido (" << bytes_recebidos
	   << ") nao corresponde com o esperado (" << tamanho << ")" << endl;
  }
  else
  {
    Excecoes ex = tempo_receber;
    throw ex;
  }

  return( bytes_recebidos );
}

int Conexao::Enviar(void *msg, int tamanho)const
{
  int bytes_enviados;


  bytes_enviados=sendto(SocketConexao,(char*) msg, tamanho,
                        0,(struct sockaddr*)&infConexao,
                        sizeof(infConexao)                 );

  if ( bytes_enviados == -1 )
    printf("Impossivel enviar dados\n");
  if ( bytes_enviados != tamanho )
    printf("Dados nao foram enviados corretamente\n");

  return(bytes_enviados);
}
