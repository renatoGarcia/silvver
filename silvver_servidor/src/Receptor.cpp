#include "Receptor.h"
#include "conexao.h"
#include <boost/bind.hpp>
#include <iostream>

using namespace std;

auto_ptr<Receptor> Receptor::instanciaUnica;
mutex Receptor::mutexInstanciacao;

Receptor* Receptor::Instanciar()
{
  mutex::scoped_lock lock(mutexInstanciacao);
  if( instanciaUnica.get()==0 )
      instanciaUnica.reset(new Receptor);

  return instanciaUnica.get();
}

Receptor::Receptor():
RECEPCIONISTA_PORTA(12000)
{
  portaLivre = 12001;
  entradas  = Entradas::Instanciar();
  saidas = Saidas::Instanciar();
  thRecepcionista = NULL;
  ftime(&tempoInicial);
}

double Receptor::tempoDecorrido()
{
  struct timeb tempoAtual;
  ftime(&tempoAtual);

  return( (double)(tempoAtual.time - tempoInicial.time) + (double)(tempoAtual.millitm - tempoInicial.millitm)/1000 );
}

void Receptor::CriarRecepcionista()
{
  if(thRecepcionista == NULL)
  {
    thRecepcionista = new thread( bind(&Receptor::Recepcionista, this) );
  }
  else
  {
    cerr << "Solicitada a criação do recepcionista mas ele já foi criado" << endl;
  }
}

int Receptor::FecharRecepcionista()
{
  //delete thRecepcionista;

  //thRecepcionista->

  return 0;
}

void Receptor::Recepcionista(Receptor *objeto)
{
  char msg[3];

  Conexao conexao;
  conexao.Iniciar(objeto->RECEPCIONISTA_PORTA);

  while(true)
  {
    conexao.Receber( msg,sizeof(msg) );// Recebe a primeira mensagem

    if( strcmp(msg,"TP") == 0 ) //Tempo Atual
    {
      double tempoAtual = objeto->tempoDecorrido();
      conexao.Enviar( &tempoAtual, sizeof(tempoAtual) );
    }
    else if( strcmp(msg,"PT") == 0 ) //Nova camera
    {
      Conexao *conexaoEntrada = new Conexao();
      conexaoEntrada->Iniciar(objeto->portaLivre);

      conexao.Enviar( &objeto->portaLivre,sizeof(objeto->portaLivre) );

      objeto->entradas->AdicionarEntrada( conexaoEntrada );

      objeto->portaLivre++;
    }
    else if( strcmp(msg,"SD") == 0 ) //Nova saída
    {
      Conexao *conexaoSaida = new Conexao();
      conexaoSaida->Iniciar(objeto->portaLivre);

      conexao.Enviar( &objeto->portaLivre,sizeof(objeto->portaLivre) );

      objeto->saidas->AdicionarSaida( conexaoSaida );

      objeto->portaLivre++;
    }
    else if( strcmp(msg,"DC") == 0 ) //Desconectar saída
    {
      int id;
      char OK[3] = "OK";
      conexao.Receber( &id,sizeof(id) );
      objeto->saidas->RetirarSaida(id);
      conexao.Enviar( OK,sizeof(OK) );
      cout << "Retirado cliente id: " << id << endl;
    }
  }
}
