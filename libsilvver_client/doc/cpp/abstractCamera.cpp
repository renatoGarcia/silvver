#include <cstdio>
#include <iostream>

#include <silvver/silvverTypes.hpp>
#include <silvver/abstractCamera.hpp>

using namespace silvver;
using namespace std;

int main(int argc, char *argv[])
{
  AbstractCamera<Pose> camera(AbstractCameraUid(1,1));
  CameraReading<Pose> reading;
  std::vector<Identity<Pose> >::iterator itPose;

  for (int i = 0; i < 10; ++i)
  {
    reading = camera.getUnseen();

    cout << "Timestamp: " << reading.timestamp << endl;
    for (itPose = reading.localizations.begin();
         itPose < reading.localizations.end();
         ++itPose)
    {
      cout << *itPose << endl;
    }
    cout << endl;
  }

  return 0;
}
