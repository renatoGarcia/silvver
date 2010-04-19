#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include <silvver/silvverTypes.hpp>
#include <silvver/rawTarget.hpp>

using namespace silvver;
using namespace std;

void printPose(Identity<Pose> pose)
{
  cout << pose << endl;
}

int main(int argc, char *argv[])
{
  unsigned id = 1;
  std::string ip("127.0.0.1");
  unsigned receptionistPort = 12000;

  if (argc > 1)
  {
    id = atoi(argv[1]);
  }
  if (argc > 2)
  {
    ip = argv[2];
  }
  if (argc > 3)
  {
    receptionistPort = atoi(argv[3]);
  }

  RawTarget<Pose> rawTarget(printPose, id, ip, receptionistPort);

  rawTarget.connect();

  cout << "Press [enter] to quit." << endl;
  getchar();

  target.disconnect();

  return 0;
}
