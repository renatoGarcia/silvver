#include <cstdlib>
#include <iostream>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define HAS_OPENCV
#include <silvver/silvverTypes.hpp>
#include <silvver/abstractCamera.hpp>

using namespace silvver;
using namespace std;

int main(int argc, char *argv[])
{
  AbstractCamera<Pose> camera(AbstractCameraUid(1,1));
  CameraReading<Pose> reading;

  reading = camera.getUnseen();
  IplImage* image = reading.image.toIplImage();
  if (image != NULL)
  {
    cvSaveImage("image.jpg", image);
    cout << "Saved" << endl;
  }
  else
  {
    cout << "Empty image" << endl;
  }

  cvReleaseImage(&image);

  return 0;
}
