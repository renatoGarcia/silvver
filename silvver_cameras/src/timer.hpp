#ifndef TIMER_HPP
#define TIMER_HPP

#include <sys/timeb.h>
#include <time.h>

#define TIMEB timeb
#define FTIME ftime

class Timer
{
private:

  // Offset a ser somado nos cálculos de tempo atual;
  double offset;
  struct TIMEB t;
  double t0, t1, t2;

public:

  double iniciar(double tempoInicial=0.0)
  {
    this->offset = tempoInicial;

    FTIME(&t);
    t0 = (double)(t.time + t.millitm/1000.0);
    return t0;
  }

  double instante()
  {
    FTIME(&t);
    t2 = (double)(t.time + t.millitm/1000.0);
    return t2;
  }

  void iniciarIntervalo()
  {
    FTIME(&t);
    t1 = (double)(t.time + t.millitm/1000.0);
  }

  double duracaoIntervalo()
  {
    FTIME(&t);
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
