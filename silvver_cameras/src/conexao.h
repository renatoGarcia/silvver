#include <iostream>
/**************************************************************************************
    Efetua a conexão do cliente
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

    // Identifica para comunicação com o servidor.
    SOCKET SocketConexao;

    // Define as caracteristicas do cliente
	sockaddr_in infConexao;

    // SenderAddr.sin_family conterá o endereço
	sockaddr_in SenderAddr;
    
    // Tamanhos em bytes de sockaddr_in SenderAddr.
    int SenderAddrSize;

public:

    Conexao();

    int Iniciar(int porta, char *ip);

    // Envia mensagem: msg é a mensagem e tamanho é o tamanho (sizeof) da mensagem
	int Enviar(void *msg, int tamanho);

    int Receber(char *msg, int tamanho);
};

#endif