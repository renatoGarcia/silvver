#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "sceneMounter.hpp"

namespace po = boost::program_options;

int main(int argc, char **argv)
{
  std::string serverIP;
  int receptionistPort;
  std::string xmlFile;

  po::options_description desc("silver_cameras 0.2\n\n"
                               "Capture images with the cameras and process it\n"
                               "Usage: silver_cameras [OPTIONS]...\n\n"
                               "Options list");
  desc.add_options()
    ("help,h", "Print this help message and exit")
    ("version,V", "Print version and exit")
    ("server-ip",
     po::value<std::string>(&serverIP)->default_value("127.0.0.1"),
     "IP address of silver_servidor")
    ("receptionist-port,p",
     po::value<int>(&receptionistPort)->default_value(12000),
     "Port on the server where the receptionist is hearing")
    ("scene-config,c",
     po::value<std::string>(&xmlFile)->default_value("scene.xml"),
     "XML file with the scene configuration")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help"))
  {
    std::cout << desc << std::endl;
  }
  else if(vm.count("version"))
  {
    std::cout << "silver_cameras 0.2" << std::endl;
  }
  else
  {
    std::cout << "silver_cameras 0.2:" << std::endl << std::endl
              << "Capture images with the cameras and process it" << std::endl
              << "Press [enter] key to quit" << std::endl << std::endl
              << "----------------------------------------------"
              << std::endl << std::endl;

    SceneMounter sceneMounter(serverIP,receptionistPort,xmlFile);
    sceneMounter.mount();

    getchar();
    std::cout << "Quitting..." << std::endl << std::endl;

    sceneMounter.dismount();
  }

  return 0;
}
