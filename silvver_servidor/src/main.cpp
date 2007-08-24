#include "Receptor.h"
#include <iostream>

using namespace std;

int main()
{
  Receptor *receptor = Receptor::Instanciar();
  receptor->CriarRecepcionista();

  char ch = getchar();
  cout << "Terminando..." << endl << endl;

  return 0;
}
