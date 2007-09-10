#ifndef TIMER_H
#define TIMER_H

#include <sys/timeb.h>
#include <time.h>

class Timer
{
private:

    // Offset a ser somado nos cálculos de tempo atual;
    double offset;
	struct _timeb t;
	double t0, t1, t2;
	
public:

	double iniciar(double tempoInicial=0.0) 
    {	
        this->offset = tempoInicial;

		_ftime(&t);	
		t0 = (double)(t.time + t.millitm/1000.0);
		return t0;
	}
	
	double instante() 
    {	
		_ftime(&t);	
		t2 = (double)(t.time + t.millitm/1000.0);
		return t2;
	}

    void iniciarIntervalo()
    {
        _ftime(&t);	
		t1 = (double)(t.time + t.millitm/1000.0);
    }

    double duracaoIntervalo()
    {
        _ftime(&t);
        t2 = (double)(t.time + t.millitm/1000.0);
        return t2-t1;
    }    

    // Valor do tempo atual, dado o tempo decorrido desde o início mais o offset
	double tempoAbsoluto()
    {								
		instante();
		return ( t2-t0 + offset );
	}	
};


#endif