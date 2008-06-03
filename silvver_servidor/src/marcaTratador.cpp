#include <iostream>
#include "marcaTratador.hpp"

mutex MarkerProcessor::mutexInstanciacao;
auto_ptr<MarkerProcessor> MarkerProcessor::instanciaUnica;

MarkerProcessor* MarkerProcessor::instantiate()
{
    mutex::scoped_lock lock(mutexInstanciacao);
    if( instanciaUnica.get()==0 )
        instanciaUnica.reset(new MarkerProcessor);

    return instanciaUnica.get();
}

MarkerProcessor::MarkerProcessor()
{
}

void MarkerProcessor::EntregarPacotes(Pacote<Ente> &pacote)
{
  vector<Ente> vecEnte;
  int idBlobCam = pacote.GetCameraId();

  pacote.Desempacotar(vecEnte);

  mutex::scoped_lock lock(mutexArmazenador);
  armazenador[idBlobCam] = vecEnte;

}

void MarkerProcessor::localize(vector<Ente> &vecRobos)
{
  vecRobos.clear();
  vector<Ente> vecEnteTemp;
  vector<Ente>::iterator iteEnteTemp;

  //---------------Copia os entes de todas as marcaCameras para vecEnte
  TMapa::iterator iteMapa = armazenador.begin();
  for(mutex::scoped_lock lock(mutexArmazenador); iteMapa != armazenador.end(); iteMapa++)
  {
    vecRobos.insert(vecRobos.end(),iteMapa->second.begin(),iteMapa->second.end());
    iteMapa->second.clear();
  }

  if(vecRobos.size() > 0)
  {
    std::cout << vecRobos.size() << std::endl;
  }
  //---------------Verifica se há um mesmo robô reportado por duas câmeras diferentes
  vector<Ente>::iterator itePrimeiro,iteSegundo;

  for(itePrimeiro = vecRobos.begin(); itePrimeiro < vecRobos.end(); itePrimeiro++)
  {
    for(iteSegundo=itePrimeiro+1; iteSegundo < vecRobos.end(); iteSegundo++)
    {
      if(iteSegundo->id == itePrimeiro->id)
      {
        itePrimeiro->Fundir(*iteSegundo);
        vecRobos.erase(iteSegundo);
      }
    }
  }
}
