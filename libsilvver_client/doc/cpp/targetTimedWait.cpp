#include <boost/date_time/time_duration.hpp>
#include <iostream>

#include <silvver/silvverTypes.hpp>
#include <silvver/target.hpp>

using namespace boost::posix_time;
using namespace silvver;
using namespace std;

int main(int argc, char **argv)
{
  Target<Pose> target(TargetUid(1,1));
  Pose pose;

  try
  {
    pose = target.getNext(seconds(5));
    cout << pose << endl;
  }
  catch (const silvver::time_expired_error& e)
  {
    cout << "Wait time expired" << endl;
  }

  return 0;
}
