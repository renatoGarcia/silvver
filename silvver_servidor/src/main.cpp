#include "receptionist.hpp"
#include <iostream>
#include <boost/scoped_ptr.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

bool verbose;

int main(int argc, char **argv)
{
  int receptionistPort;

  po::options_description desc("silver_servidor 0.1\n\n"
                               "Receive inforamation from silver_cameras and send to clients\n"
                               "Usage: silver_servidor [OPTIONS]...\n\n"
                               "Options list");
  desc.add_options()
    ("help,h", "Print this help message and exit")
    ("version,V", "Print version and exit")
    ("receptionist-port,p",
     po::value<int>(&receptionistPort)->default_value(12000),
     "Port where the receptionist will hear")
    ("verbose,v", "verbose mode")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help"))
  {
    std::cout << desc << std::endl;
    return 1;
  }

  if(vm.count("version"))
  {
    std::cout << "silver_servidor 0.1" << std::endl;
    return 1;
  }

  if(vm.count("verbose"))
  {
    verbose = true;
  }

  boost::scoped_ptr<Receptionist> receptionist(new Receptionist(receptionistPort));
  receptionist->run();

  getchar();
  std::cout << "Terminando..." << std::endl << std::endl;

  return 0;
}
