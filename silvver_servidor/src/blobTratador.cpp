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

void BlobTratador::EntregarPacotes(Pacote<Marca> &pacote)
{
  vector<Marca> vecMarca;
  int idBlobCam = pacote.GetCameraId();

  pacote.Desempacotar(vecMarca);

  mutex::scoped_lock lock(mutexArmazenador);
  armazenador[idBlobCam] = vecMarca;
}

void BlobTratador::Localizar(vector<Ente> &vecRobo)
{
  vecRobo.clear();

  vector<Marca> vecCorUnica,vecCorComum;

  if ( SepararCores(vecCorUnica,vecCorComum) ) return;
  EliminarDuplos(vecCorUnica,vecCorComum);
  if ( CasarParesDeCor(vecCorUnica,vecCorComum) ) return;

  vector<Marca>::iterator iteCorUnica,iteCorComum,fimCorUnica,fimCorComum;
  iteCorUnica = vecCorUnica.begin();
  iteCorComum = vecCorComum.begin();
  fimCorUnica = vecCorUnica.end();
  fimCorComum = vecCorComum.end();

  for(;(iteCorUnica!=fimCorUnica) && (iteCorComum!=fimCorComum);iteCorUnica++,iteCorComum++)
  {
    vecRobo.push_back( Ente(*iteCorComum,*iteCorUnica,false) );
  }
}

int BlobTratador::SepararCores(vector<Marca> &vecCorUnica,vector<Marca> &vecCorComum)
{
  vecCorUnica.clear();
  vecCorComum.clear();

  vector<Marca> vecMarca;

  //---------------Copia as marcas de todas as blobCameras para vecMarca
  TMapa::iterator iteMapa,fimMapa;
  iteMapa = armazenador.begin();
  fimMapa = armazenador.end();
  for(mutex::scoped_lock lock(mutexArmazenador);iteMapa != fimMapa; iteMapa++)
  {
    vecMarca.insert(vecMarca.end(),iteMapa->second.begin(),iteMapa->second.end());
  }
  //--------------------------------

  vector<Marca>::iterator iteVecMarca,fimVecMarca;
  iteVecMarca = vecMarca.begin();
  fimVecMarca = vecMarca.end();
  Marca marca;

  for(;iteVecMarca != fimVecMarca; iteVecMarca++)
  {
    marca = *iteVecMarca;

    if(iteVecMarca->id == COR_COMUM)
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

void BlobTratador::EliminarDuplos(vector<Marca> &vecCorUnica,vector<Marca> &vecCorComum)
{
  vector<Marca>::iterator itePrimeiro,iteSegundo;

  double distancia;

  //--------------- Cor �nica --------------------//
  itePrimeiro = vecCorUnica.begin();
  for(;itePrimeiro < vecCorUnica.end(); itePrimeiro++)
  {
    for(iteSegundo=itePrimeiro+1; iteSegundo<vecCorUnica.end(); iteSegundo++)
    {
      distancia = itePrimeiro->CalcularDistancia(*iteSegundo);
      if(distancia < DISTANCIA_FUSAO)
      {
        itePrimeiro->Fundir(*iteSegundo);
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
      distancia = itePrimeiro->CalcularDistancia(*iteSegundo);
      if(distancia < DISTANCIA_FUSAO)
      {
        itePrimeiro->Fundir(*iteSegundo);
        vecCorUnica.erase(iteSegundo);
      }
    }
  }
}

int BlobTratador::CasarParesDeCor(vector<Marca> &vecCorUnica,vector<Marca> &vecCorComum)
{
  if( (vecCorUnica.size()!= vecCorComum.size()) || vecCorUnica.empty() ) return -1;

  double distancia,distanciaMinima;
  Marca marcaTemporaria;
  vector<Marca>::iterator iteCorUnica,
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
      distancia = iteCorUnica->CalcularDistancia(*iteCorComum2);
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
