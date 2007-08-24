#include "conexao.h"

Conexao::Conexao()
{
    this->SenderAddrSize = sizeof(SenderAddr);
}

int Conexao::Iniciar(int porta)
{
    int erro;

  #ifdef HAVE_WINDOWS_SOCKETS
    WSADATA initialisation_win32;

    erro = WSAStartup(MAKEWORD(2,2),&initialisation_win32);
    if(erro)
    {
        printf("Suporte a sockets indisponivel.\n");
        return 1;
    }
  #endif

    // Cria��o de um Socket
    SocketConexao = socket(AF_INET,SOCK_DGRAM,0);
    if (SocketConexao<=0)
    {
        printf("Imposs�vel de Criar Socket.\n");
        return 1;
    }

    // Inicializa��o do Servidor
    infConexao.sin_family = AF_INET;         //Indica a utiliza��o do IPv4
    infConexao.sin_addr.s_addr = INADDR_ANY; //Ecoute sur toutes les IP locales
    infConexao.sin_port = htons(porta);      //porta na qual o cliente estar� escutando

    if( bind(SocketConexao,(struct sockaddr*)&infConexao,sizeof(infConexao)) == -1 )
    {
        perror("bind");
        return 1;
    }

	SenderAddr.sin_family = 0;

    return 0;
}

int Conexao::Receber(void *msg, int tamanho)
{
	int bytes_recebidos;

	bytes_recebidos = recvfrom(SocketConexao,(char*) msg, tamanho,
                                0, (struct sockaddr*)&SenderAddr,
                                &SenderAddrSize);

	if ( bytes_recebidos == -1 )
		printf("Erro d: A mensagem nao foi recebida corretamente.\n"/*,
		WSAGetLastError()*/ );
	if ( bytes_recebidos != tamanho )
		printf("O tamanho do dado recebido (%d) nao corresponde com o"
			" esperado (%d).\n", bytes_recebidos, tamanho);

	return( bytes_recebidos );
}

int Conexao::Enviar(void *msg, int tamanho)
{
    int bytes_enviados;

    bytes_enviados = sendto(SocketConexao,(char*) msg, tamanho,
                            0,(struct sockaddr*)&SenderAddr,
                            sizeof(SenderAddr));

    if ( bytes_enviados == -1 )
		printf("Impossivel enviar dados\n");
	if ( bytes_enviados != tamanho )
		printf("Dados nao foram enviados corretamente\n");

	return(bytes_enviados);
}
