#include "conexao.hpp"
#include <iostream>

Conexao::Conexao()
{
  this->SenderAddrSize = sizeof(SenderAddr);
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
  SocketConexao = socket(AF_INET,SOCK_DGRAM,0);
  if (SocketConexao<=0)
  {
    std::cerr << "Erro ao criar o socket" << std::endl;
    return 1;
  }

  // Iniciação do Cliente
  infConexao.sin_family = AF_INET;              // Indica a utilização do IPv4
  infConexao.sin_addr.s_addr = inet_addr(ip);   // Endereço IP do servidor
  infConexao.sin_port = htons(porta);           // Porta utiliazada
//   SenderAddr=infConexao;

  return 0;
}

int Conexao::Receber(char *msg, int tamanho)
{
  int bytes_recebidos;

  bytes_recebidos = recvfrom(SocketConexao, msg, tamanho,
                             0, (struct sockaddr*)&SenderAddr,
                             &SenderAddrSize);

  if ( bytes_recebidos == -1 )
    std::cerr << "A mensagem nao foi recebida corretamente" << std::endl;
  if ( bytes_recebidos != tamanho )
    std::cerr << "O tamanho do dado recebido (" << bytes_recebidos
              << ") nao corresponde com o esperado (" << tamanho << ")"
              << std::endl;

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
