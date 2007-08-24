#include "marcaTratador.h"

mutex MarcaTratador::mutexInstanciacao;
auto_ptr<MarcaTratador> MarcaTratador::instanciaUnica;

MarcaTratador* MarcaTratador::Instanciar()
{
    mutex::scoped_lock lock(mutexInstanciacao);
    if( instanciaUnica.get()==0 )
        instanciaUnica.reset(new MarcaTratador);

    return instanciaUnica.get();
}

MarcaTratador::MarcaTratador()
{
}

void MarcaTratador::EntregarPacotes(Pacote<Ente> &pacote)
{
  vector<Ente> vecEnte;
  int idBlobCam = pacote.GetCameraId();

  pacote.Desempacotar(vecEnte);

  mutex::scoped_lock lock(mutexArmazenador);
  armazenador[idBlobCam] = vecEnte;

}

void MarcaTratador::Localizar(vector<Ente> &vecRobos)
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
