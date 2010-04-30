#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include <silvver/silvverTypes.hpp>
#include <silvver/abstractCamera.hpp>

using namespace silvver;
using namespace std;

void printPose(CameraReading<Pose> reading)
{
  std::vector<Identity<Pose> >::iterator itPose;

  cout << "Timestamp: " << reading.timestamp << endl;
  for (itPose = reading.localizations.begin();
       itPose < reading.localizations.end();
       ++itPose)
  {
    cout << *itPose << endl;
  }
  cout << endl;
}

int main(int argc, char *argv[])
{
  AbstractCameraUid uid(1,1);

  AbstractCamera<Pose> camera(printPose, uid);

  camera.connect();

  cout << "Press [enter] to quit." << endl;
  getchar();

  camera.disconnect();

  return 0;
}
