#include <iostream>
#include <vector>

#include <silvver/silvverTypes.hpp>
#include <silvver/targetSet.hpp>

using namespace silvver;
using namespace std;

int main(int argc,char **argv)
{
  TargetSet<Pose> targetSet(TargetSetUid(1));
  vector<Identity<Pose> > vecPose;

  for (int i = 0; i < 10; ++i)
  {
    vecPose = targetSet.getUnseen();
    for(vector<Identity<Pose> >::iterator it = vecPose.begin();
        it != vecPose.end();
        ++it)
    {
      cout << *it << endl;
    }
  }

  return 0;
}
