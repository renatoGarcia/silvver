#include <iostream>

#include <silvver/silvverTypes.hpp>
#include <silvver/target.hpp>

using namespace silvver;
using namespace std;

int main(int argc,char **argv)
{
  Target<Pose> target(TargetUid(1,1));
  Pose pose;

  for (int i = 0; i < 10; ++i)
  {
    pose = target.getUnseen();
    cout << static_cast<Position>(pose) << '\t' << pose.theta() << endl;
  }

  return 0;
}
