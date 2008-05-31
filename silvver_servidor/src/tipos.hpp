/**************************************************************************************
    Especifica��o dos tipos usados para a comunica��o entre as classes
    Versão 1.0
*************************************************************************************/
#ifndef TIPOS_HPP
#define TIPOS_HPP

#define PORTA_RECEPCIONISTA 12000
#define PORTA_BROADCAST 11999

#include <math.h>
#include <vector>
#include <iterator>

namespace verlab
{

/*----------------------------POSI��O---------------------*/
class Posicao
{
public:
    double x;
    double y;
public:
    Posicao():x(0.0),y(0.0){}

    Posicao(double x,double y)
    {
        this->x = x;
        this->y = y;
    }

    Posicao(const Posicao& posicao)
    {
        this->x = posicao.x;
        this->y = posicao.y;
    }

    double CalcularDistancia(const Posicao &posicao)
    {
        return sqrt( pow(this->x-posicao.x, 2) + pow(this->y-posicao.y, 2) );
    }

    /* O �ngulo ser� um valor entre [-PI,+PI] e ser� tomado tendo-se o objeto   |
    |  this como ponto de refer�ncia. Em uma situa��o em que se tenha uma reta  |
    |  horizontal, com posicao � direita de this, o �nguro ter� um valor de     |
    |  zero Toda inclina��o ser� medida como se this mantivesse-se fixo.       */
    double CalcularAngulo(Posicao posicao)
    {
        return( atan2(posicao.y - this->y,posicao.x - this->x) );
    }
};

/*----------------------------POSE------------------------*/
class Pose: public Posicao
{
public:
  double teta;
public:
  Pose():Posicao(),teta(0.0){}

  Pose(double x,double y,double teta)
  :Posicao(x,y)
  {
    this->teta = teta;
  }

  Pose(Posicao posicao,double teta)
  :Posicao(posicao)
  {
    this->teta = teta;
  }

  Pose(const Pose& pose)
  {
    this->x    = pose.x;
    this->y    = pose.y;
    this->teta = pose.teta;
  }
};

/*---------------------------MARCA------------------------*/
class Marca: public Posicao
{
public:
  int id;
  float peso;
  double tempo;
public:
  Marca():Posicao(),id(0),peso(0.0){}

  Marca(double x,double y,int id,float peso,double tempo=0.0)
  :Posicao(x,y)
  {
    this->id   = id;
    this->peso = peso;
    this->tempo = tempo;
  }

  Marca(Posicao posicao,int id,float peso)
  :Posicao(posicao)
  {
    this->id = id;
    this->peso = peso;
    this->tempo = tempo;
  }

  Marca(const Marca& marca)
  :Posicao(marca.x,marca.y)
  {
    this->id   = marca.id;
    this->peso = marca.peso;
    this->tempo = marca.tempo;
  }

  void setTempo(double tempo)
  {
    this->tempo = tempo;
  }

  void Fundir(const Marca& marca) //TODO: Levar o tempo em consideração no cálculo do peso
  {
    double maiorTempo = (this->tempo > marca.tempo) ? this->tempo : marca.tempo;

    this->x     = (this->x*this->peso + marca.x*marca.peso)/(this->peso+marca.peso);
    this->y     = (this->y*this->peso + marca.y*marca.peso)/(this->peso+marca.peso);
    this->peso  = (this->peso + marca.peso)/2;
    this->tempo = maiorTempo;
  }
};

/*-----------------------------ENTE-----------------------*/
class Ente: public Marca
{
public:
    double teta;
public:
    Ente():Marca(),teta(0.0){}

    Ente(double x,double y,double teta,int id,float peso,double tempo=0.0)
    :Marca(x,y,id,peso,tempo)
    {
        this->teta = teta;
    }

    Ente(Marca marca,double teta)
    :Marca(marca)
    {
        this->teta = teta;
    }

    Ente(const Ente& ente)
    :Marca(ente.x, ente.y, ente.id, ente.peso, ente.tempo)
    {
        this->teta = ente.teta;
    }

  // idMarcaReferencia � true se o id ser� tomado da marcaRefer�ncia ou false
  // caso for tomado de marca2
  Ente(Marca marcaReferencia,Marca marca2,bool idMarcaReferencia)
  :Marca()
  {
    double maiorTempo = (marcaReferencia.tempo > marca2.tempo) ? marcaReferencia.tempo : marca2.tempo;

    this->id = idMarcaReferencia ? marcaReferencia.id : marca2.id;

    this->x    = (marcaReferencia.x*marcaReferencia.peso + marca2.x*marca2.peso)
                 /(marcaReferencia.peso+marca2.peso);
    this->y    = (marcaReferencia.y*marcaReferencia.peso + marca2.y*marca2.peso)
                 /(marcaReferencia.peso+marca2.peso);
    this->peso = (marcaReferencia.peso + marca2.peso)/2;

    this->teta = atan2(marca2.y - marcaReferencia.y,marca2.x - marcaReferencia.x);

    this->tempo = maiorTempo;
  }
};

/*------------------------PACOTE--------------------------*/
#define MAX_ITENS 30

template <class Tipo>
class Pacote
{
private:
    Tipo item[MAX_ITENS];
    unsigned short totalItens;
    int cameraId;
public:
    Pacote(int cameraId=0)
    {
        this->cameraId = cameraId;
    }

    void Empacotar(std::vector<Tipo> &vecItens)
    {
      this->totalItens = vecItens.size();
      typename std::vector<Tipo>::iterator iteVecItens = vecItens.begin();
      for(int i=0; iteVecItens<vecItens.end(); iteVecItens++,i++)
      {
          item[i] = *iteVecItens;
      }
    }

  void Desempacotar(std::vector<Tipo> &vecItens)
  {
    for(int i=0;i<totalItens;i++)
    {
      vecItens.push_back(item[i]);
    }
  }

    int GetCameraId(){return cameraId;}

    unsigned short GetTotalItens(){return totalItens;}
};

}//namespace verlab

#endif
