#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

#include <silvver/silvverTypes.hpp>
#include <silvver/target.hpp>

using namespace silvver;

void printTargets(boost::shared_ptr<Target<Pose> > target)
{
  silvver::Pose pose;

  while(!boost::this_thread::interruption_requested())
  {
    pose = target->getNew();
    std::cout << pose << std::endl;
  }
}

int main(int argc,char **argv)
{
  unsigned id = 1;
  std::string ip("127.0.0.1");
  unsigned receptionistPort = 12000;

  if (argc > 1)
  {
    if ((std::string(argv[1]) == "-h") || (std::string(argv[1]) == "--help"))
    {
      std::cout << "silvver_client" << std::endl << std::endl
                << "Print the target pose until [enter] key be pressed." << std::endl
                << "Execution:" << std::endl
                << "silvver_client [target id] [server ip] [receptionist port]"
                << std::endl;
      return 0;
    }
    else
    {
      id = atoi(argv[1]);
    }
  }
  if (argc > 2)
  {
    ip = argv[2];
  }
  if (argc > 3)
  {
    receptionistPort = atoi(argv[3]);
  }

  boost::shared_ptr<Target<Pose> > target(new Target<Pose>(id,
                                                           ip,
                                                           receptionistPort));

  std::cout << "Connecting to server..." << std::endl;
  target->connect();
  std::cout << "Connected."<< std::endl << std::endl;

  std::cout << "Press [enter] to quit." << std::endl;

  boost::thread th(boost::bind(&printTargets, target));

  getchar();

  th.interrupt();
  th.join();

  return 0;
}