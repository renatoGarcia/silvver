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
  cout << reading.localizations.at(0) << endl;
}

int main(int argc, char *argv[])
{
  std::string id = "1";
  std::string ip("127.0.0.1");
  unsigned receptionistPort = 12000;

  if (argc > 1)
  {
    id = argv[1];
  }
  if (argc > 2)
  {
    ip = argv[2];
  }
  if (argc > 3)
  {
    receptionistPort = atoi(argv[3]);
  }

  AbstractCamera<Pose> camera(printPose, id, ip, receptionistPort);

  camera.connect();

  cout << "Press [enter] to quit." << endl;
  getchar();

  camera.disconnect();

  return 0;
}
