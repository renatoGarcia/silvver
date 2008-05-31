#include <iostream>
/**************************************************************************************

*************************************************************************************/

#ifndef MARCATRATADOR_HPP
#define MARCATRATADOR_HPP

#include <map>
#include <vector>
#include <boost/thread/mutex.hpp>
#include "tipos.hpp"

using namespace std;
using namespace verlab;
using namespace boost;

class MarcaTratador
{
private:

    static auto_ptr<MarcaTratador> instanciaUnica;
    static mutex mutexInstanciacao;
    MarcaTratador();

    typedef map< unsigned,vector<Ente> > TMapa;

    // Armazenara os ultimos entes obtidos de cada marcaCamera, onde
    // a chave e um identificador destas.
    TMapa armazenador;

    mutex mutexArmazenador;

public:

    static MarcaTratador* Instanciar();

    void EntregarPacotes(Pacote<Ente> &pacote);

    // Calcula a configura��o dos rob�s, usando os dados atualmente
    // dispon�veis no map armazenador.
    void Localizar(vector<Ente> &vecRobos);
};

#endif
