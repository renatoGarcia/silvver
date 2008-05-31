#include "receptor.hpp"
#include "connection.hpp"
#include <boost/bind.hpp>
#include <iostream>

using namespace std;

extern bool verbose;
#define VERBOSE_PRINT(msg) if(verbose)cout<<msg;

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
    VERBOSE_PRINT("Waiting message\n");
    conexao.Receber( msg,sizeof(msg) );// Recebe a primeira mensagem

    if( strcmp(msg,"TP") == 0 ) //Tempo Atual
    {
      VERBOSE_PRINT("Receive message: TP (actual time)\n");
      double tempoAtual = objeto->tempoDecorrido();
      conexao.Enviar( &tempoAtual, sizeof(tempoAtual) );
    }
    else if( strcmp(msg,"PT") == 0 ) //Nova camera
    {
      VERBOSE_PRINT("Receive message: PT (new camera)\n");
      Conexao *conexaoEntrada = new Conexao();
      conexaoEntrada->Iniciar(objeto->portaLivre);

      conexao.Enviar( &objeto->portaLivre,sizeof(objeto->portaLivre) );

      objeto->entradas->AdicionarEntrada( conexaoEntrada );

      objeto->portaLivre++;
    }
    else if( strcmp(msg,"SD") == 0 ) //Nova saída
    {
      VERBOSE_PRINT("Receive message: SD (connect client)\n");
      Conexao *conexaoSaida = new Conexao();
      conexaoSaida->Iniciar(objeto->portaLivre);

      conexao.Enviar( &objeto->portaLivre,sizeof(objeto->portaLivre) );

      objeto->saidas->AdicionarSaida( conexaoSaida );

      objeto->portaLivre++;
    }
    else if( strcmp(msg,"DC") == 0 ) //Desconectar saída
    {
      VERBOSE_PRINT("Receive message: DC (disconnect client)\n");
      int id;
      char OK[3] = "OK";
      conexao.Receber( &id,sizeof(id) );
      objeto->saidas->RetirarSaida(id);
      conexao.Enviar( OK,sizeof(OK) );
      cout << "Retirado cliente id: " << id << endl;
    }
    else
    {
      std::cerr << "Unknown message: " << msg[0] << msg[1] << std::endl;
    }
  }
}
