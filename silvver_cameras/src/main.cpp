/* Copyright 2009 Renato Florentino Garcia <fgar.renato@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/program_options.hpp>
#include <boost/version.hpp>
#include <iostream>
#include <string>

#include "sceneMounter.hpp"

namespace po = boost::program_options;

int main(int argc, char **argv)
{
  std::string serverIP;
  int receptionistPort;
  std::string luaFile;

  po::options_description desc("silvver_cameras 0.4\n\n"
                               "Compiled with boost version " BOOST_LIB_VERSION "\n\n"

                               "Capture images with the cameras and process it\n"
                               "Usage: silvver_cameras [OPTIONS]...\n\n"
                               "Options list");
  desc.add_options()
    ("help,h", "Print this help message and exit")
    ("version,V", "Print version and exit")
    ("server-ip",
     po::value<std::string>(&serverIP)->default_value("127.0.0.1"),
     "IP address of silvver_servidor")
    ("receptionist-port,p",
     po::value<int>(&receptionistPort)->default_value(12000),
     "Port on the server where the receptionist is hearing")
    ("scene-config,c",
     po::value<std::string>(&luaFile)->default_value("scene.lua"),
     "Lua file with the scene configuration")
    ("show-images,s", "Show the captured images by cameras")
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
    std::cout << "silvver_cameras 0.4" << std::endl;
  }
  else
  {
    std::cout << "silvver_cameras 0.4:\n\n"
              << "Compiled with boost version " << BOOST_LIB_VERSION << '\n'
              << "Capture images with the cameras and process it\n"
              << "Press [enter] key to quit\n\n"
              << "----------------------------------------------\n"
              << std::endl;

    SceneMounter sceneMounter(serverIP,receptionistPort,luaFile);
    sceneMounter.mount(vm.count("show-images"));

    getchar();
    std::cout << "Quitting..." << std::endl << std::endl;
  }

  return 0;
}
