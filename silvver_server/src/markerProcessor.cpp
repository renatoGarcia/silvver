/* Copyright 2009 Renato Florentino Garcia <fgar.renato@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include "markerProcessor.hpp"
#include "processor.ipp"

MarkerProcessor::MarkerProcessor()
  :Processor<silvver::Identity<silvver::Pose>,
             silvver::Identity<silvver::Pose> >::Processor()
{}

void
MarkerProcessor::deliverPackage(const std::vector< silvver::Identity<silvver::Pose> > &pacote,
                                const unsigned id)
{
  boost::mutex::scoped_lock lock(mutexArmazenador);
  this->lastInputs[id] = pacote;

  this->localize();
}

void
MarkerProcessor::localize()
{
  std::vector<silvver::Identity<silvver::Pose> > vecRobos;

  std::vector<silvver::Identity<silvver::Pose> > vecEnteTemp;
  std::vector<silvver::Identity<silvver::Pose> >::iterator iteEnteTemp;

  //---------------Copia os entes de todas as marcaCameras para vecEnte
  TMap::iterator iteMapa = this->lastInputs.begin();

  for(; iteMapa != this->lastInputs.end(); iteMapa++)
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
  std::vector<silvver::Identity<silvver::Pose> >::iterator itePrimeiro,iteSegundo;

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
