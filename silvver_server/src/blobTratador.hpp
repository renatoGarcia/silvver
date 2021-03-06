#include <iostream>
/**************************************************************************************
    Efetua a conex�o do servidor
**************************************************************************************/

#ifndef BLOBTRATADOR_HPP
#define BLOBTRATADOR_HPP

#include <limits.h>
#include <map>
#include <vector>
#include <boost/thread/mutex.hpp>
#include "silverTypes.hpp"

using namespace std;
using namespace silver;
using namespace boost;

class BlobTratador
{
private:

  static auto_ptr<BlobTratador> instanciaUnica;

  static mutex mutexInstanciacao;

  BlobTratador();

  typedef map< unsigned,vector<Blob> > TMapa;

  // M�xima dist�ncia para que duas manchas sejam consideradas uma.
  const double DISTANCIA_FUSAO;

  // A id da cor comum. � definida com 1, a primeira entrada de cor.
  const int COR_COMUM;

  // Armazenar� os �ltimos blobs obtidos de cada blobC�mera, onde
  // a chave � um identificador desta.
  TMapa armazenador;

  mutex mutexArmazenador;

  // Coloca todos os blobs em seu vetor adequado (vecCorUnica ou vecCorComum).
  int SepararCores(vector<Blob> &vecCorUnica,vector<Blob> &vecCorComum);

  // Procura por manchas que foram reportadas por mais de uma c�mera, e a converte
  // em uma �nica.
  void EliminarDuplos(vector<Blob> &vecCorUnica,vector<Blob> &vecCorComum);

  //    Encontra para cada cor �nica a cor comum que est� a uma menor dist�ncia em 
  //  rela��o � pr�pria. Ap�s ter encontrado, essa cor comum ter� sua posi��o em
  //  'vecCorComum' mudada para uma posi��o igual a da cor �nica relacionada em
  //  'vecCorUnica'.
  int CasarParesDeCor(vector<Blob> &vecCorUnica,vector<Blob> &vecCorComum);

public:

  static BlobTratador* Instanciar();

  void EntregarPacotes(Package<Blob> &pacote, unsigned id);

  // Calcula a configura��o dos rob�s, usando os dados atualmente
  // dispon�veis no map armazenador.
  void Localizar(vector<Ente> &vecRobos);
};

#endif
