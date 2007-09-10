#include "conexao.h"

Conexao::Conexao()
{
    this->SenderAddrSize = sizeof(SenderAddr);    
}

int Conexao::Iniciar(int porta, char *ip)
{
    int erro;

  #ifdef WINDOWS_SOCKET
    WSADATA initialisation_win32;

    erro = WSAStartup(MAKEWORD(2,2),&initialisation_win32);
    if(erro)
    {
        printf("Suporte a sockets indisponível.\n");
        return 1;
    }
  #endif

    // Criação de um Socket 
    SocketConexao = socket(AF_INET,SOCK_DGRAM,0); 
    if (SocketConexao==INVALID_SOCKET) 
    {
        printf("Impossível de Criar Socket.\n"); 
        return 1;
    }

    // Inicialização do Cliente
    infConexao.sin_family=AF_INET;              // Indica a utilização do IPv4 
    infConexao.sin_addr.s_addr = inet_addr(ip); //Endereço IP dos servidor
    infConexao.sin_port=htons(porta);           // Porta utiliazada 
//	SenderAddr=infConexao;

	return 0;
}

int Conexao::Receber(char *msg, int tamanho)
{
	int bytes_recebidos;

    
	bytes_recebidos = recvfrom(SocketConexao, msg, tamanho, 
                               0, (struct sockaddr*)&SenderAddr, 
                               &SenderAddrSize);   

	if ( bytes_recebidos == SOCKET_ERROR )
		printf("Erro %d: A mensagem nao foi recebida corretamente.\n",
		WSAGetLastError() );
	if ( bytes_recebidos != tamanho )
		printf("O tamanho do dado recebido (%d) nao corresponde com o"
			" esperado (%d).\n", bytes_recebidos, tamanho);

	return( bytes_recebidos );
}

int Conexao::Enviar(void *msg, int tamanho)
{
	int bytes_enviados;	
	
	
	bytes_enviados=sendto(SocketConexao,(char*) msg, tamanho,
                            0,(struct sockaddr*)&infConexao,
                            sizeof(infConexao));
	
	if ( bytes_enviados == SOCKET_ERROR ) 
		printf("Impossivel enviar dados\n");
	if ( bytes_enviados != tamanho )
		printf("Dados nao foram enviados corretamente\n");
	
	return(bytes_enviados);
}
