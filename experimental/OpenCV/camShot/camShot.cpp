#ifdef __APPLE__
#include <OpenCV/OpenCV.h>
#endif

#ifdef __linux__
#include "cv.h"
#include "highgui.h"
#endif

//#include <highgui.h>

int main ( int argc, char **argv )
{
  CvCapture* capture = cvCreateCameraCapture(0);
  IplImage* pframe = cvQueryFrame(capture);
  cvShowImage("Cam Shot", pframe);
  cvWaitKey();
  cvSaveImage("camShot.png",pframe);
  return 0;
}
