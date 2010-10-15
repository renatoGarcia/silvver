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

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/any.hpp>
#include <boost/program_options.hpp>
#include <boost/version.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include "common/connection/tcpIp.hpp"
#include "common/version.hpp"
#include "globalOptions.hpp"
#include "log.hpp"
#include "sceneMounter.hpp"

namespace po = boost::program_options;

globalOptions::Options global_options;

// Overloaded boost program_options function to validate LogLevel
void validate(boost::any& v,
              const std::vector<std::string>& values,
              LogLevel* verbosityLevel, int)
{
  po::validators::check_first_occurrence(v);

  std::string s = po::validators::get_single_string(values);
  boost::to_upper(s);

  LogLevel::optional verbosity = LogLevel::get_by_name(s.c_str());
  if (verbosity)
  {
    v = boost::any(*verbosity);
  }
  else
  {
    std::cerr << "Error: Invalid verbosity level " + s + ". Aborting."
              << std::endl;
    exit(EXIT_FAILURE);
  }
}

std::string getLevelsNames()
{
  std::string levelsNames;
  for(LogLevel::const_iterator iterLevel = LogLevel::begin();
      iterLevel < LogLevel::end();
      ++iterLevel)
  {
    levelsNames += '|';
    levelsNames += iterLevel->str();
  }
  boost::erase_first(levelsNames, "|");

  return levelsNames;
}

int main(int argc, char **argv)
{
  std::string serverName;
  std::string receptionistPort;
  std::string luaFile;
  LogLevel verbosity;

  po::options_description desc("silvver_cameras " SILVVER_LIB_VERSION " \n\n"
                               "Compiled with boost version " BOOST_LIB_VERSION "\n\n"
                               "Capture images with the cameras and process it\n"
                               "Usage: silvver_cameras [OPTIONS]...\n\n"
                               "Options list");
  desc.add_options()
    ("help,h", "Print this help message and exit")
    ("version,V", "Print version and exit")
    ("server-name,s",
     po::value<std::string>(&serverName)->default_value("localhost"),
     "Hostname or IP address of silvver-server")
    ("receptionist-port,p",
     po::value<std::string>(&receptionistPort)->default_value("12000"),
     "Port on the server where the receptionist is hearing")
    ("scene-config,c",
     po::value<std::string>(&luaFile)->default_value("scene.lua"),
     "Lua file with the scene configuration")
    ("verbosity,v",
     po::value<LogLevel>(&verbosity)->default_value(LogLevel::WARN),
     std::string("Minimum level of messages. ["  +
                 getLevelsNames() + ']').c_str())
    ("show", "Show the images captured by cameras")
    ("save-unwarped",
     "Save the images captured by cameras with intrinsic distortion corrected")
    ("save-warped",
     "Save the images captured by cameras without correct the intrinsic distortion")
    ("save-timestamp", "Save a file with timestamps of saved images")
    ;

  po::variables_map vm;

  try
  {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  }
  catch(const boost::program_options::multiple_occurrences& e)
  {
    std::cerr << "Error: multiple occurrences of one command line option."
              << std::endl;
    exit(EXIT_FAILURE);
  }

  if(vm.count("help"))
  {
    std::cout << desc << std::endl;
  }
  else if(vm.count("version"))
  {
    std::cout << "silvver_cameras " SILVVER_LIB_VERSION << std::endl;
  }
  else
  {
    std::cout << "silvver_cameras " SILVVER_LIB_VERSION ":\n\n"
              << "Compiled with boost version " << BOOST_LIB_VERSION << '\n'
              << "Capture images with the cameras and process it\n"
              << "Press [enter] key to quit\n\n"
              << "----------------------------------------------\n"
              << std::endl;

    global_options.receptionistEp = connection::TcpIp::resolve(serverName, receptionistPort).front();
    global_options.showImages = vm.count("show");
    global_options.saveWarpedImages = vm.count("save-warped");
    global_options.saveUnwarpedImages = vm.count("save-unwarped");
    global_options.saveTimestamp = vm.count("save-timestamp");
    message.setThreshold(verbosity.value());

    SceneMounter sceneMounter(luaFile);
    sceneMounter.mount();

    getchar();
    std::cout << "Quitting..." << std::endl << std::endl;
  }

  return EXIT_SUCCESS;
}
