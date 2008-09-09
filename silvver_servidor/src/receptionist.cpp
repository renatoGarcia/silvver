#include "receptionist.hpp"
#include "connection.hpp"
#include <boost/ref.hpp>
#include <iostream>
#include "inputFactory.hpp"

extern bool verbose;
#define VERBOSE_PRINT(msg) if(verbose)std::cout<<msg;

Receptionist::Receptionist(int port)
  :stop(false)
  ,RECECPTIONIST_PORT(port)
{
  this->freePort = port+1;
  this->outputs = ClientsMap::instantiate();
  ftime(&this->startTime);
}

Receptionist::~Receptionist()
{
  this->stop = true;
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

  while(!this->stop)
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

      boost::shared_ptr<Connection>
        inputConnection(new Connection(this->freePort));
      inputConnection->initialize();

      connection.send(&this->freePort,sizeof(this->freePort));

      char msg[3];
      inputConnection->receive(msg, sizeof(msg));
      std::cout << "Confirma conexao: " << msg << std::endl;

      InputType inputType;
      inputConnection->receive( &inputType,sizeof(InputType) );

      boost::shared_ptr<InputInterface> input =
        InputFactory::createInput(inputType, inputConnection);

      input->confirmConnect();
      input->run();

      this->mapInputs.insert(std::pair< unsigned,boost::shared_ptr<InputInterface> >
                             (this->freePort, input));

      this->freePort++;
    }
    else if(strcmp(msg,"SD") == 0 ) //Nova saída
    {
      VERBOSE_PRINT("Receive message: SD (connect client)\n");
      boost::shared_ptr<Connection>
        outputConnection(new Connection(this->freePort));
      outputConnection->initialize();

      connection.send(&this->freePort,sizeof(this->freePort));

      this->outputs->addOutput(outputConnection);

      this->freePort++;
    }
    else if(strcmp(msg,"DC") == 0) //Desconectar saída
    {
      VERBOSE_PRINT("Receive message: DC (disconnect client)\n");
      unsigned targetId, connectionPort;

      connection.receive(&targetId, sizeof(targetId));
      connection.receive(&connectionPort, sizeof(connectionPort));
      this->outputs->removeOutput(targetId, connectionPort);

      std::cout << "Retirado cliente id: " << targetId
                << " na porta: "<< connectionPort << std::endl;
    }
    else
    {
//       std::cerr << "Unknown message: " << msg[0] << msg[1] << std::endl;
    }
    msg[0]=0;msg[1]=0;msg[2]=0;
  }
}
