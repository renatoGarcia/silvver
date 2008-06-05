#include "receptionist.hpp"
#include <iostream>
#include "cmdline.h"
#include <boost/scoped_ptr.hpp>

using namespace std;

bool verbose;

int main(int argc, char **argv)
{
  gengetopt_args_info args_info;
  if (cmdline_parser (argc, argv, &args_info) != 0)
    return 1;

  // int portaRecepcionista = args_info.porta_recepcionista_arg;
  verbose = args_info.verbose_flag;

  boost::scoped_ptr<Receptionist> receptionist(new Receptionist());
  receptionist->run();

  getchar();
  cout << "Terminando..." << endl << endl;

  return 0;
}
