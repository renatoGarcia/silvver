#include "receptor.hpp"
#include <iostream>
#include "cmdline.h"

using namespace std;

bool verbose;

int main(int argc, char **argv)
{
  gengetopt_args_info args_info;
  if (cmdline_parser (argc, argv, &args_info) != 0)
    return 1;

  // int portaRecepcionista = args_info.porta_recepcionista_arg;
  verbose = args_info.verbose_flag;

  Receptor *receptor = Receptor::Instanciar();
  receptor->CriarRecepcionista();

  getchar();
  cout << "Terminando..." << endl << endl;

  return 0;
}
