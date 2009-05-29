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
  :Processor<silver::Identity<silver::Pose>,
             silver::Identity<silver::Pose> >::Processor()
{
}

void
MarkerProcessor::deliverPackage(std::vector<silver::Identity<silver::Pose> > &pacote,
                                unsigned id)
{
  boost::mutex::scoped_lock lock(mutexArmazenador);
  armazenador[id] = pacote;

  this->localize();
}

void
MarkerProcessor::localize()
{
  std::vector<silver::Identity<silver::Pose> > vecRobos;

  std::vector<silver::Identity<silver::Pose> > vecEnteTemp;
  std::vector<silver::Identity<silver::Pose> >::iterator iteEnteTemp;

  //---------------Copia os entes de todas as marcaCameras para vecEnte
  TMapa::iterator iteMapa = armazenador.begin();

  for(; iteMapa != armazenador.end(); iteMapa++)
  {
    vecRobos.insert(vecRobos.end(),iteMapa->second.begin(),iteMapa->second.end());
    iteMapa->second.clear();
  }

//   if(vecRobos.size() > 0)
//   {
//     std::cout << "ID: " << vecRobos.at(0).id << std::endl;
//     std::cout << "X: " << vecRobos.at(0).x << std::endl;
//   }

  //---------------Verifica se há um mesmo robô reportado por duas câmeras diferentes
  std::vector<silver::Identity<silver::Pose> >::iterator itePrimeiro,iteSegundo;

  for(itePrimeiro = vecRobos.begin(); itePrimeiro < vecRobos.end(); itePrimeiro++)
  {
    for(iteSegundo=itePrimeiro+1; iteSegundo < vecRobos.end(); iteSegundo++)
    {
      if(iteSegundo->uid == itePrimeiro->uid)
      {
        itePrimeiro->x = (itePrimeiro->x + iteSegundo->x) / 2;
        itePrimeiro->y = (itePrimeiro->y + iteSegundo->y) / 2;

        vecRobos.erase(iteSegundo);
      }
    }
  }

  this->sendToOutputs(vecRobos);
}
