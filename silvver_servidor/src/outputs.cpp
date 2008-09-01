#include "outputs.hpp"
#include <iostream>

boost::shared_ptr<Outputs> Outputs::singleInstance;
boost::mutex Outputs::instantiatingMutex;

boost::shared_ptr<Outputs>
Outputs::instantiate()
{
  boost::mutex::scoped_lock lock(Outputs::instantiatingMutex);
  if(!Outputs::singleInstance)
  {
    Outputs::singleInstance.reset(new Outputs);
  }

  return Outputs::singleInstance;
}

Outputs::Outputs()
{
  logFile.reset(new std::ofstream("log.txt"));
}

void
Outputs::addOutput(Connection *outputConnection)
{
  char msg[3];
  char OK[3] = "OK";
  int clientId;

  outputConnection->receive(msg,sizeof(msg)); //Deverá receber como resposta a mensagem "OK".
  std::cout << "Confirma conexao: " << msg << std::endl;

  outputConnection->receive( &clientId,sizeof(int) );
  std::cout << "ID: " << clientId << std::endl;

  client[clientId] = outputConnection;

  client.find(clientId)->second->send(OK,sizeof(OK));
}

void
Outputs::sendResults(const std::vector<silver::Ente> &vecRobo)
{
  boost::mutex::scoped_lock lock(sendResultsMutex);

  std::vector<silver::Ente>::const_iterator iteEnte;

  for(iteEnte = vecRobo.begin(); iteEnte < vecRobo.end(); iteEnte++)
  {
    *logFile << iteEnte->id << '\t' << iteEnte->x  << '\t'
             << iteEnte->y << '\t' << iteEnte->theta <<  std::endl;

    //Caso exista um cliente com o mesmo id do Ente apontado por iteEnte envia a pose para ele.
    if(client.find(iteEnte->id) != client.end())
    {
      client.find(iteEnte->id)->second->send((void*)&(*iteEnte),
                                               sizeof(*iteEnte) );
    }
  }
}

void
Outputs::removeOutput(int id)
{
  this->client.erase(this->client.find(id));
}
