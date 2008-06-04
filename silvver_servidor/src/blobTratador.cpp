#include "blobTratador.hpp"

mutex BlobTratador::mutexInstanciacao;
auto_ptr<BlobTratador> BlobTratador::instanciaUnica;

BlobTratador* BlobTratador::Instanciar()
{
  mutex::scoped_lock lock(mutexInstanciacao);
  if( instanciaUnica.get()==0 )
    instanciaUnica.reset(new BlobTratador);

  return instanciaUnica.get();
}

BlobTratador::BlobTratador():
DISTANCIA_FUSAO(10),
COR_COMUM(1)
{
}

void BlobTratador::EntregarPacotes(Package<Blob> &pacote, unsigned id)
{
  vector<Blob> vecBlob;

  pacote.unpack(vecBlob);

  mutex::scoped_lock lock(mutexArmazenador);
  armazenador[id] = vecBlob;
}

void BlobTratador::Localizar(vector<Ente> &vecRobo)
{
  vecRobo.clear();

  vector<Blob> vecCorUnica,vecCorComum;

  if ( SepararCores(vecCorUnica,vecCorComum) ) return;
  EliminarDuplos(vecCorUnica,vecCorComum);
  if ( CasarParesDeCor(vecCorUnica,vecCorComum) ) return;

  vector<Blob>::iterator iteCorUnica,iteCorComum,fimCorUnica,fimCorComum;
  iteCorUnica = vecCorUnica.begin();
  iteCorComum = vecCorComum.begin();
  fimCorUnica = vecCorUnica.end();
  fimCorComum = vecCorComum.end();

  for(;(iteCorUnica!=fimCorUnica) && (iteCorComum!=fimCorComum);iteCorUnica++,iteCorComum++)
  {
    vecRobo.push_back( Ente(*iteCorComum,*iteCorUnica,false) );
  }
}

int BlobTratador::SepararCores(vector<Blob> &vecCorUnica,vector<Blob> &vecCorComum)
{
  vecCorUnica.clear();
  vecCorComum.clear();

  vector<Blob> vecBlob;

  //---------------Copia as marcas de todas as blobCameras para vecBlob
  TMapa::iterator iteMapa,fimMapa;
  iteMapa = armazenador.begin();
  fimMapa = armazenador.end();
  for(mutex::scoped_lock lock(mutexArmazenador);iteMapa != fimMapa; iteMapa++)
  {
    vecBlob.insert(vecBlob.end(),iteMapa->second.begin(),iteMapa->second.end());
  }
  //--------------------------------

  vector<Blob>::iterator iteVecBlob,fimVecBlob;
  iteVecBlob = vecBlob.begin();
  fimVecBlob = vecBlob.end();
  Blob marca;

  for(;iteVecBlob != fimVecBlob; iteVecBlob++)
  {
    marca = *iteVecBlob;

    if(iteVecBlob->id == COR_COMUM)
    {
      vecCorComum.push_back(marca);
    }
    else
    {
      vecCorUnica.push_back(marca);
    }
  }

  return 0;
}

void BlobTratador::EliminarDuplos(vector<Blob> &vecCorUnica,vector<Blob> &vecCorComum)
{
  vector<Blob>::iterator itePrimeiro,iteSegundo;

  double distancia;

  //--------------- Cor �nica --------------------//
  itePrimeiro = vecCorUnica.begin();
  for(;itePrimeiro < vecCorUnica.end(); itePrimeiro++)
  {
    for(iteSegundo=itePrimeiro+1; iteSegundo<vecCorUnica.end(); iteSegundo++)
    {
      distancia = itePrimeiro->findDistance(*iteSegundo);
      if(distancia < DISTANCIA_FUSAO)
      {
        itePrimeiro->fuse(*iteSegundo);
        vecCorUnica.erase(iteSegundo);
      }
    }
  }

  //--------------- Cor comum --------------------//
  itePrimeiro = vecCorComum.begin();
  for(;itePrimeiro < vecCorComum.end(); itePrimeiro++)
  {
    for(iteSegundo=itePrimeiro+1; iteSegundo<vecCorComum.end(); iteSegundo++)
    {
      distancia = itePrimeiro->findDistance(*iteSegundo);
      if(distancia < DISTANCIA_FUSAO)
      {
        itePrimeiro->fuse(*iteSegundo);
        vecCorUnica.erase(iteSegundo);
      }
    }
  }
}

int BlobTratador::CasarParesDeCor(vector<Blob> &vecCorUnica,vector<Blob> &vecCorComum)
{
  if( (vecCorUnica.size()!= vecCorComum.size()) || vecCorUnica.empty() ) return -1;

  double distancia,distanciaMinima;
  Blob marcaTemporaria;
  vector<Blob>::iterator iteCorUnica,
                          iteCorComum1,
                          iteCorComum2,
                          fimVecCorUnica,
                          fimVecCorComum,
                          parCorUnica;

  iteCorUnica    = vecCorUnica.begin();
  iteCorComum1   = vecCorComum.begin();
  fimVecCorUnica = vecCorUnica.end();
  fimVecCorComum = vecCorComum.end();

  for(;iteCorUnica != fimVecCorUnica; iteCorUnica++,iteCorComum1++)
  {
    iteCorComum2 = iteCorComum1;
    for(distanciaMinima=LONG_MAX; iteCorComum2!=fimVecCorComum; iteCorComum2++ )
    {
      distancia = iteCorUnica->findDistance(*iteCorComum2);
      if(distancia<distanciaMinima)
      {
        parCorUnica = iteCorComum2;
        distanciaMinima = distancia;
      }
    }
    marcaTemporaria = *iteCorComum1;
    *iteCorComum1   = *parCorUnica;
    *parCorUnica    = marcaTemporaria;
  }
  return 0;
}
