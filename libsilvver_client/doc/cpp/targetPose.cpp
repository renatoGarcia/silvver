#include <cstdlib>
#include <iostream>
#include <string>

#include <silvver/silvverTypes.hpp>
#include <silvver/target.hpp>

using namespace silvver;
using namespace std;

int main(int argc,char **argv)
{
  TargetUid uid(1,1);

  Target<Pose> target(uid);
  Pose pose;

  for (int i = 0; i < 10; ++i)
  {
    pose = target.getNext();
    cout << static_cast<Position>(pose) << '\t' << pose.theta() << endl;
  }

  return 0;
}
