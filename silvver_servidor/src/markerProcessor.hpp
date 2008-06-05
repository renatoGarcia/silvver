#ifndef MARKER_PROCESSOR_HPP
#define MARKER_PROCESSOR_HPP

#include <map>
#include <vector>
#include <boost/thread/mutex.hpp>
#include "silverTypes.hpp"

using namespace std;
using namespace silver;
using namespace boost;

class MarkerProcessor
{
public:

  static MarkerProcessor* instantiate();

  void deliverPackage(Package<Ente> &pacote, unsigned id);

  // Calcula a configuraçãoo dos robôs, usando os dados atualmente
  // disponíveis no map armazenador.
  void localize(vector<Ente> &vecRobos);

private:

  static auto_ptr<MarkerProcessor> instanciaUnica;
  static mutex mutexInstanciacao;
  MarkerProcessor();

  typedef map<unsigned,vector<Ente> > TMapa;

  // Armazenara os ultimos entes obtidos de cada marcaCamera, onde
  // a chave e um identificador destas.
  TMapa armazenador;

  mutex mutexArmazenador;

};

#endif
