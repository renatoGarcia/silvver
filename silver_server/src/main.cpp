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

#include <iostream>

#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>

#include "debugOutputs.hpp"
#include "receptionist.hpp"

namespace po = boost::program_options;

int main(int argc, char **argv)
{
  tsostream ts(std::cout.rdbuf());
  int receptionistPort;
  unsigned verbosityLevel;
  bool logInputs;

  po::options_description desc("silver_server 0.4\n\n"
                               "Receive inforamation from silver_cameras and send to clients\n"
                               "Usage: silver_server [OPTIONS]...\n\n"
                               "Options list");
  desc.add_options()
    ("help,h", "Print this help message and exit")
    ("version,V", "Print version and exit")
    ("receptionist-port,p",
     po::value<int>(&receptionistPort)->default_value(12000),
     "Port where the receptionist will hear")
    ("verbosity,v",
     po::value<unsigned>(&verbosityLevel)->default_value(1),
     "Verbosity level")
    ("log-inputs,l", "Print the received localizations.")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
  }
  else if(vm.count("version"))
  {
    std::cout << "silver_server 0.4" << std::endl;
  }
  else
  {
    debug::messageOutput.setThreshold(verbosityLevel);

    debug::logOut.setThreshold(vm.count("log-inputs"));

    debug::messageOutput(STARTUP)
      << "silver_server 0.4:\n\n"
      << "Press [enter] key to quit\n\n"
      << "----------------------------------------------\n"
      << std::endl;

    boost::scoped_ptr<Receptionist>
      receptionist(new Receptionist(receptionistPort));
    receptionist-> run();

    getchar();
    debug::messageOutput(STARTUP)
      << "Quitting..." << std::endl << std::endl;
  }
  return 0;
}
