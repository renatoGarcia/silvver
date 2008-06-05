#include "receptionist.hpp"
#include "connection.hpp"
#include <boost/ref.hpp>
#include <iostream>

extern bool verbose;
#define VERBOSE_PRINT(msg) if(verbose)cout<<msg;

Receptionist::Receptionist()
  :stopReceptionist(false)
  ,RECECPTIONIST_PORT(12000)
{
  this->freePort = 12001;
  this->inputs.reset(new Inputs());
  this->outputs.reset(Saidas::Instanciar());
  ftime(&this->startTime);
}

Receptionist::~Receptionist()
{
  this->stopReceptionist = true;
  this->thReceptionist->join();
}

double
Receptionist::elapsedTime()
{
  struct timeb instantTime;
  ftime(&instantTime);

  return( (double)(instantTime.time - startTime.time) +
          (double)(instantTime.millitm - startTime.millitm)/1000 );
}

void
Receptionist::run()
{
  if(!this->thReceptionist)
  {
    thReceptionist.reset(new boost::thread(boost::ref(*this)));
  }
}

void
Receptionist::operator()()
{
  char msg[3];

  Connection connection(this->RECECPTIONIST_PORT);
  connection.initialize();

  while(!this->stopReceptionist)
  {
    connection.receive(msg, sizeof(msg), 1);// Recebe a primeira mensagem

    if(strcmp(msg,"TP") == 0) //Tempo Atual
    {
      VERBOSE_PRINT("Receive message: TP (actual time)\n");
      double instantTime = this->elapsedTime();
      connection.send(&instantTime, sizeof(instantTime));
    }
    else if(strcmp(msg,"PT") == 0) //Nova camera
    {
      VERBOSE_PRINT("Receive message: PT (new camera)\n");
      Connection *inputConnection = new Connection(this->freePort);
      inputConnection->initialize();

      connection.send(&this->freePort,sizeof(this->freePort));

      this->inputs->addInput(inputConnection);

      this->freePort++;
    }
    else if(strcmp(msg,"SD") == 0 ) //Nova saída
    {
      VERBOSE_PRINT("Receive message: SD (connect client)\n");
      Connection *outputConnection = new Connection(this->freePort);
      outputConnection->initialize();

      connection.send(&this->freePort,sizeof(this->freePort));

      this->outputs->AdicionarSaida(outputConnection);

      this->freePort++;
    }
    else if(strcmp(msg,"DC") == 0) //Desconectar saída
    {
      VERBOSE_PRINT("Receive message: DC (disconnect client)\n");
      int id;
      char OK[3] = "OK";
      connection.receive(&id,sizeof(id));
      this->outputs->RetirarSaida(id);
      connection.send(OK,sizeof(OK));
      cout << "Retirado cliente id: " << id << endl;
    }
    else
    {
      std::cerr << "Unknown message: " << msg[0] << msg[1] << std::endl;
    }
    msg[0]=0;msg[1]=0;msg[2]=0;
  }
}
