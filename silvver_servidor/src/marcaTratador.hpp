#ifndef MARKER_PROCESSOR_HPP
#define MARKER_PROCESSOR_HPP

#include <map>
#include <vector>
#include <boost/thread/mutex.hpp>
#include "tipos.hpp"

using namespace std;
using namespace verlab;
using namespace boost;

class MarkerProcessor
{
public:

    static MarkerProcessor* instantiate();

    void EntregarPacotes(Pacote<Ente> &pacote);

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
