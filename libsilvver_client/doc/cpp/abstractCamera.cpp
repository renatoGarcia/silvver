#include <cstdio>
#include <iostream>

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
  AbstractCamera<Pose> camera(AbstractCameraUid(1,1));
  camera.setCallback(printPose);

  cout << "Press [enter] to quit." << endl;
  getchar();

  return 0;
}
