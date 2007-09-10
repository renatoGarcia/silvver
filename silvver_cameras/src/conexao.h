#include <iostream>
/**************************************************************************************
    Efetua a conex�o do cliente
**************************************************************************************/
#ifndef CONEXAO_H
#define CONEXAO_H

#define WINDOWS_SOCKET

#ifdef WINDOWS_SOCKET
#include <winsock.h>
#include <stdio.h>
#endif

class Conexao
{
private:

    // Identifica para comunica��o com o servidor.
    SOCKET SocketConexao;

    // Define as caracteristicas do cliente
	sockaddr_in infConexao;

    // SenderAddr.sin_family conter� o endere�o
	sockaddr_in SenderAddr;
    
    // Tamanhos em bytes de sockaddr_in SenderAddr.
    int SenderAddrSize;

public:

    Conexao();

    int Iniciar(int porta, char *ip);

    // Envia mensagem: msg � a mensagem e tamanho � o tamanho (sizeof) da mensagem
	int Enviar(void *msg, int tamanho);

    int Receber(char *msg, int tamanho);
};

#endif