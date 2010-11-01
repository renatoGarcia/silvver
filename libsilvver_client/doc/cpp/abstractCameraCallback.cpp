#include <cstdio>
#include <iostream>

#include <silvver/abstractCamera.hpp>
#include <silvver/callbackWrapper.hpp>
#include <silvver/silvverTypes.hpp>

using namespace silvver;
using namespace std;

void printPose(CameraReading<Pose> reading, ErrorCode ec)
{
  if (ec)
  {
    cout << "Error" << endl;
    return;
  }

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
  CallbackWrapper<AbstractCamera<Pose> > cb(new AbstractCamera<Pose>(AbstractCameraUid(1,1)),
                                            printPose);

  cout << "Press [enter] to quit." << endl;
  getchar();

  return 0;
}
