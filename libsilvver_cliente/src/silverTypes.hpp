/**
 * @file   silverTypes.hpp
 *
 * @brief Especificação dos tipos e constantes usados para a comunicação entre os módulos de silvver.
 *
 * Versão 0.2
 */
#ifndef SILVER_TYPES_HPP
#define SILVER_TYPES_HPP

#include <math.h>
#include <vector>
#include <iterator>

namespace silver
{

  /*----------------------------POSITION---------------------*/
  class Position
  {
  public:
    double x;
    double y;
  public:
    Position()
    {
      this->x = 0.0;
      this->y = 0.0;
    }

    Position(double x, double y)
    {
      this->x = x;
      this->y = y;
    }

    Position(const Position& position)
    {
      this->x = position.x;
      this->y = position.y;
    }

    double findDistance(const Position &position)
    {
      return sqrt(pow(this->x - position.x, 2) + pow(this->y - position.y, 2));
    }

    /**
     * O ângulo será um valor entre [-PI,+PI] e será tomado tendo-se o objeto
     * this como ponto de referência. Em uma situação em que se tenha uma reta
     * horizontal, com posicao à direita de this, o ânguro terá um valor de
     * zero Toda inclinação será medida como se this mantivesse-se fixo.
     * @param position
     *
     * @return
     */
    double findAngle(const Position &position)
    {
      return(atan2(position.y - this->y, position.x - this->x));
    }
  };

  /*----------------------------POSE------------------------*/
  class Pose: public Position
  {
  public:
    double theta;
  public:
    Pose()
      :Position()
    {
      this->theta = 0.0;
    }

    Pose(double x, double y, double theta)
      :Position(x,y)
    {
      this->theta = theta;
    }

    Pose(const Position &position, double theta)
      :Position(position)
    {
      this->theta = theta;
    }

    Pose(const Pose& pose)
      :Position(pose)
    {
      this->theta = pose.theta;
    }
  };

  /*---------------------------BLOB------------------------*/
  class Blob: public Position
  {
  public:
    int id;
    float weigh;
  public:
    Blob()
      :Position()
    {
      this-> id   = 0;
      this-> weigh = 0.0;
    }

    Blob(double x, double y, int id, float weigh)
      :Position(x,y)
    {
      this->id   = id;
      this->weigh = weigh;
    }

    Blob(const Position &position, int id, float weigh)
      :Position(position)
    {
      this->id   = id;
      this->weigh = weigh;
    }

    Blob(const Blob& blob)
      :Position(blob)
    {
      this->id    = blob.id;
      this->weigh = blob.weigh;
    }

    void fuse(const Blob& blob)
    {
      this->x     = (this->x * this->weigh + blob.x * blob.weigh)/
                    (this->weigh+blob.weigh);
      this->y     = (this->y * this->weigh + blob.y * blob.weigh)/
                    (this->weigh+blob.weigh);
      this->weigh = (this->weigh + blob.weigh)/2;
    }
  };

  /*-----------------------------ENTE-----------------------*/
  class Ente: public Blob
  {
  public:
    double theta;
  public:
    Ente()
      :Blob()
    {
      this->theta = 0.0;
    }

    Ente(double x, double y, double theta, int id, float weigh)
      :Blob(x, y, id, weigh)
    {
      this->theta = theta;
    }

    Ente(const Blob &blob, double theta)
      :Blob(blob)
    {
      this->theta = theta;
    }

    Ente(const Ente& ente)
      :Blob(ente)
    {
      this->theta = ente.theta;
    }

    // idBlobReference é true se o id será tomado da blobReferência ou false
    // caso for tomado de blob2
    Ente(Blob blobReference,Blob blob2,bool idBlobReference)
      :Blob()
    {
      this->id = idBlobReference ? blobReference.id : blob2.id;

      this->x    = (blobReference.x*blobReference.weigh + blob2.x*blob2.weigh)
                   /(blobReference.weigh+blob2.weigh);
      this->y    = (blobReference.y*blobReference.weigh + blob2.y*blob2.weigh)
                   /(blobReference.weigh+blob2.weigh);
      this->weigh = (blobReference.weigh + blob2.weigh)/2;

      this->theta = atan2(blob2.y - blobReference.y,blob2.x - blobReference.x);
    }
  };

  /*------------------------PACKAGE--------------------------*/
#define MAX_ITEMS 30

  template <class Type>
  class Package
  {
  private:
    Type item[MAX_ITEMS];
    unsigned short usedItems;
  public:
    Package()
    {}

    void pack(std::vector<Type> &vecItems)
    {
      this->usedItems = vecItems.size();
      typename std::vector<Type>::iterator iteVecItems = vecItems.begin();
      for(int i=0; iteVecItems<vecItems.end(); iteVecItems++,i++)
      {
	item[i] = *iteVecItems;
      }
    }

    void unpack(std::vector<Type> &vecItems)
    {
      for(int i=0;i<usedItems;i++)
      {
	vecItems.push_back(item[i]);
      }
    }

    unsigned short getUsedItems(){return usedItems;}
  };

}//namespace silver

#endif
