/* Copyright 2009, 2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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
#include <boost/scoped_ptr.hpp>
#include <boost/version.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include "log.hpp"
#include "receptionist.hpp"

#define VERSION "0.5.0"

namespace po = boost::program_options;

void validate(boost::any& v,
              const std::vector<std::string>& values,
              MessageLogLevel* messageLogLevel, int)
{
  po::validators::check_first_occurrence(v);

  std::string s = po::validators::get_single_string(values);
  boost::to_upper(s);

  MessageLogLevel::optional level = MessageLogLevel::get_by_name(s.c_str());
  if (level)
  {
    v = boost::any(*level);
  }
  else
  {
    std::cerr << "Error: Invalid message verbosity level " + s + ". Aborting."
              << std::endl;
    exit(EXIT_FAILURE);
  }
}

std::string getMessageLogLevelsNames()
{
  std::string levelsNames;
  MessageLogLevel::const_iterator iterLevel = MessageLogLevel::begin();
  for(; iterLevel < MessageLogLevel::end(); iterLevel++)
  {
    levelsNames += '|';
    levelsNames += iterLevel->str();
  }
  boost::erase_first(levelsNames, "|");

  return levelsNames;
}

int main(int argc, char **argv)
{
  int receptionistPort;
  bool logInputs = false;
  MessageLogLevel verbosity;

  po::options_description desc("silvver_server " VERSION "\n\n"
                               "Compiled with boost version " BOOST_LIB_VERSION "\n\n"
                               "Receive inforamation from silvver_cameras and send to clients\n"
                               "Usage: silvver_server [OPTIONS]...\n\n"
                               "Options list");
  desc.add_options()
    ("help,h", "Print this help message and exit")
    ("version,V", "Print version and exit")
    ("receptionist-port,p",
     po::value<int>(&receptionistPort)->default_value(12000),
     "Port where the receptionist will hear")
    ("verbosity,v",
     po::value<MessageLogLevel>(&verbosity)->default_value(MessageLogLevel::WARN),
     std::string("Minimum level of messages. ["  +
                 getMessageLogLevelsNames() + ']').c_str())
    ("log-inputs,l",
     po::bool_switch(&logInputs),
     "Print the received localizations.")
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
    std::cout << "silvver_server " VERSION << std::endl;
  }
  else
  {
    message.setThreshold(verbosity.value());
    if (logInputs)
    {
      targetsLog.setThreshold(TargetsLogLevel::INFO);
    }
    else
    {
      targetsLog.setThreshold(TargetsLogLevel::NOTHING);
    }

    std::cout << "silvver_server " VERSION ":\n\n"
              << "Compiled with boost version " << BOOST_LIB_VERSION << '\n'
              << "Press [enter] key to quit\n\n"
              << "----------------------------------------------\n"
              << std::endl;

    boost::scoped_ptr<Receptionist>
      receptionist(new Receptionist(receptionistPort));
    receptionist-> run();

    getchar();
    std::cout << "Quitting..." << std::endl << std::endl;
  }

  return EXIT_SUCCESS;
}
