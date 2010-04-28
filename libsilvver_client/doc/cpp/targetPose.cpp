#include <cstdlib>
#include <iostream>
#include <string>

#include <silvver/silvverTypes.hpp>
#include <silvver/target.hpp>

using namespace silvver;
using namespace std;

int main(int argc,char **argv)
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

  Target<Pose> target(id, ip, receptionistPort);
  Pose pose;

  target.connect();

  for (int i = 0; i < 10; ++i)
  {
    pose = target.getNext();
    cout << (Position)pose << '\t' << pose.theta() << endl;
  }

  target.disconnect();

  return 0;
}
