#include "receptor.hpp"
#include "connection.hpp"
#include <boost/ref.hpp>
#include <iostream>

extern bool verbose;
#define VERBOSE_PRINT(msg) if(verbose)cout<<msg;

Receptionist::Receptionist()
  :stopReceptionist(false)
  ,RECEPCIONISTA_PORTA(12000)
{
  this->portaLivre = 12001;
  this->entradas.reset(Entradas::Instanciar());
  this->saidas.reset(Saidas::Instanciar());
  ftime(&this->tempoInicial);
}

Receptionist::~Receptionist()
{
  this->stopReceptionist = true;
  this->thRecepcionista->join();
}

double
Receptionist::tempoDecorrido()
{
  struct timeb tempoAtual;
  ftime(&tempoAtual);

  return( (double)(tempoAtual.time - tempoInicial.time) +
          (double)(tempoAtual.millitm - tempoInicial.millitm)/1000 );
}

void
Receptionist::run()
{
  if(!this->thRecepcionista)
  {
    thRecepcionista.reset(new boost::thread(boost::ref(*this)));
  }
}

int Receptionist::FecharRecepcionista()
{
  return 0;
}

void
Receptionist::operator()()
{
  char msg[3];

  Connection connection(this->RECEPCIONISTA_PORTA);
  connection.initialize();

  while(!this->stopReceptionist)
  {
    connection.receive( msg,sizeof(msg) );// Recebe a primeira mensagem

    if( strcmp(msg,"TP") == 0 ) //Tempo Atual
    {
      VERBOSE_PRINT("Receive message: TP (actual time)\n");
      double tempoAtual = this->tempoDecorrido();
      connection.send( &tempoAtual, sizeof(tempoAtual) );
    }
    else if( strcmp(msg,"PT") == 0 ) //Nova camera
    {
      VERBOSE_PRINT("Receive message: PT (new camera)\n");
      Connection *conexaoEntrada = new Connection(this->portaLivre);
      conexaoEntrada->initialize();

      connection.send( &this->portaLivre,sizeof(this->portaLivre) );

      this->entradas->AdicionarEntrada( conexaoEntrada );

      this->portaLivre++;
    }
    else if( strcmp(msg,"SD") == 0 ) //Nova saída
    {
      VERBOSE_PRINT("Receive message: SD (connect client)\n");
      Connection *conexaoSaida = new Connection(this->portaLivre);
      conexaoSaida->initialize();

      connection.send( &this->portaLivre,sizeof(this->portaLivre) );

      this->saidas->AdicionarSaida( conexaoSaida );

      this->portaLivre++;
    }
    else if( strcmp(msg,"DC") == 0 ) //Desconectar saída
    {
      VERBOSE_PRINT("Receive message: DC (disconnect client)\n");
      int id;
      char OK[3] = "OK";
      connection.receive( &id,sizeof(id) );
      this->saidas->RetirarSaida(id);
      connection.send( OK,sizeof(OK) );
      cout << "Retirado cliente id: " << id << endl;
    }
    else
    {
      std::cerr << "Unknown message: " << msg[0] << msg[1] << std::endl;
    }
  }
}
