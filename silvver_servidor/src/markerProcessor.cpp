#include <iostream>
#include "markerProcessor.hpp"
#include "processor_impl.hpp"

boost::mutex MarkerProcessor::instantiatingMutex;
boost::shared_ptr<MarkerProcessor> MarkerProcessor::singleInstance;

boost::shared_ptr<MarkerProcessor>
MarkerProcessor::instantiate()
{
  boost::mutex::scoped_lock lock(MarkerProcessor::instantiatingMutex);
  if(!MarkerProcessor::singleInstance)
    MarkerProcessor::singleInstance.reset(new MarkerProcessor);

  return MarkerProcessor::singleInstance;
}

MarkerProcessor::MarkerProcessor()
  :Processor<silver::Ente,silver::Ente>::Processor()
{
}

void
MarkerProcessor::deliverPackage(silver::Package<silver::Ente> &pacote,
                                unsigned id)
{
  std::vector<silver::Ente> vecEnte;

  pacote.unpack(vecEnte);

  boost::mutex::scoped_lock lock(mutexArmazenador);
  armazenador[id] = vecEnte;

  this->localize();
}

void
MarkerProcessor::localize()
{
  std::vector<silver::Ente> vecRobos;

  std::vector<silver::Ente> vecEnteTemp;
  std::vector<silver::Ente>::iterator iteEnteTemp;

  //---------------Copia os entes de todas as marcaCameras para vecEnte
  TMapa::iterator iteMapa = armazenador.begin();

  for(; iteMapa != armazenador.end(); iteMapa++)
  {
    vecRobos.insert(vecRobos.end(),iteMapa->second.begin(),iteMapa->second.end());
    iteMapa->second.clear();
  }

  if(vecRobos.size() > 0)
  {
    std::cout << "ID: " << vecRobos.at(0).id << std::endl;
    std::cout << "X: " << vecRobos.at(0).x << std::endl;
  }

  //---------------Verifica se há um mesmo robô reportado por duas câmeras diferentes
  std::vector<silver::Ente>::iterator itePrimeiro,iteSegundo;

  for(itePrimeiro = vecRobos.begin(); itePrimeiro < vecRobos.end(); itePrimeiro++)
  {
    for(iteSegundo=itePrimeiro+1; iteSegundo < vecRobos.end(); iteSegundo++)
    {
      if(iteSegundo->id == itePrimeiro->id)
      {
        itePrimeiro->fuse(*iteSegundo);
        vecRobos.erase(iteSegundo);
      }
    }
  }

  this->sendToOutputs(vecRobos);
}
