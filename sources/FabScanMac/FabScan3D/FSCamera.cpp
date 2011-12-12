//
//  FSCamera.cpp
//  FabScan3D
//
//  Created by Francis Engelmann on 7/2/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "FSCamera.h"

#include <iostream>

using namespace std;
// ---------------------------------------------
#pragma mark -
#pragma mark Constructor and Destructor
// ---------------------------------------------

FSCamera::FSCamera(const char* resPath)
{
  this->resPath = resPath;
  display = 0;
  /*
  %TODO what happens when no camera is connected
  */
  camera = cvCreateCameraCapture(CV_CAP_ANY);
  //position = FSMakePoint(0.0f, 1.0f, 5.0f);
  
  char* intrinsicsPath;
  asprintf(&intrinsicsPath,"%s/Intrinsics.xml",resPath);
  char* distortionPath;
  asprintf(&distortionPath,"%s/Distortion.xml",resPath);
  
  
  intrinsic = (CvMat*)cvLoad(intrinsicsPath);
  distortion = (CvMat*)cvLoad(distortionPath);
  
  cout << __PRETTY_FUNCTION__ <<" intrinsicMatrix" << endl;
  /*cout << CV_MAT_ELEM(*intrinsic, float, 0, 0 ) << " ";
  cout << CV_MAT_ELEM(*intrinsic, float, 0, 1 ) << " ";
  cout << CV_MAT_ELEM(*intrinsic, float, 0, 2 ) << endl;
  cout << CV_MAT_ELEM(*intrinsic, float, 1, 0 ) << " ";
  cout << CV_MAT_ELEM(*intrinsic, float, 1, 1 ) << " ";
  cout << CV_MAT_ELEM(*intrinsic, float, 1, 2 ) << endl;
  cout << CV_MAT_ELEM(*intrinsic, float, 2, 0 ) << " ";
  cout << CV_MAT_ELEM(*intrinsic, float, 2, 1 ) << " ";
  cout << CV_MAT_ELEM(*intrinsic, float, 2, 2 ) << endl; */
}

FSCamera::~FSCamera()
{
  cvReleaseCapture(&camera);
}

void FSCamera::setIntrinsic(CvMat* intrinsic)
{
  this->intrinsic = intrinsic;
}

CvMat* FSCamera::getIntrinsic()
{
  return intrinsic;
}

void FSCamera::setDistortion(CvMat* distortion)
{
  this->distortion = distortion;
}

CvMat* FSCamera::getDistortion()
{
  return distortion;
}

IplImage* FSCamera::fetchQuickFrame()
{
  return cvQueryFrame(camera);
}

IplImage* FSCamera::fetchFrame()
{
  //cvGrabFrame(camera);
  //cvRetrieveFrame(camera); //we need to do this, cos somehow 
  return cvCloneImage(cvQueryFrame(camera));
}

IplImage* FSCamera::fetchUndistortedFrame()
{
  IplImage* image = this->fetchFrame();
  
  IplImage* mapx = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );
  IplImage* mapy = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );
  cvInitUndistortMap(intrinsic, distortion, mapx, mapy);
  IplImage *t = cvCloneImage(image);
  cvRemap( t, image, mapx, mapy );
  
  return image;
}

//currently not used at all
IplImage* FSCamera::setPositionUsingChessboardCalibration(FSChessboardCalibration* cb)
{
  //CvMat* rotation = cvCreateMat(1,3,CV_64F);
  //CvMat* translation = cvCreateMat(1,3,CV_64F);
  
  //IplImage* blub = cb->findBoardInFrame2(this->fetchFrame(), intrinsic, distortion,rotation, translation);
  
  IplImage* blub = this->fetchFrame();
  position = cb->findExtrinsics(blub);
  
  //position.x = (float)CV_MAT_ELEM(*translation, double, 0, 0);
  //position.y = (float)CV_MAT_ELEM(*translation, double, 0, 1);
  //position.z = (float)CV_MAT_ELEM(*translation, double, 0, 2);
  
  /*cout << CV_MAT_ELEM(*translation, double, 0, 0) << " ";
  cout << CV_MAT_ELEM(*translation, double, 0, 1) << " ";
  cout << CV_MAT_ELEM(*translation, double, 0, 2) << endl;*/
  
  //rotation.x = CV_MAT_ELEM(*translation, float, 0, 0);
  //rotation.y = CV_MAT_ELEM(*translation, float, 0, 1);
  //rotation.z = CV_MAT_ELEM(*translation, float, 0, 2);
  
  return blub;
}

void FSCamera::draw(void)
{
  //cout << __PRETTY_FUNCTION__ << " camera draw "<< endl;
  glPushMatrix();
    glTranslated(this->position.x, this->position.y, this->position.z);
    glRotated(this->rotation.x,1,0,0);
    glRotated(this->rotation.y,0,1,0);
    glRotated(this->rotation.z,0,0,1);


    glBegin(GL_LINES);
      glVertex3f(0.0f, 0.0f, 0.0f);
      glVertex3f(0.0f, 0.0f, -position.z);
    glEnd();

    glColor3f(0.0f, 0.7f, 0.0f);
    glBegin(GL_LINES);
      glVertex3f( 0.0f,  0.0f,  0.0f);
      glVertex3f(-1.0f, -1.0f, -3.0f);
      glVertex3f( 0.0f,  0.0f, 0.0f);
      glVertex3f(-1.0f,  1.0f, -3.0f);
      glVertex3f( 0.0f,  0.0f, 0.0f);
      glVertex3f( 1.0f, -1.0f, -3.0f);
      glVertex3f( 0.0f,  0.0f, 0.0f);
      glVertex3f( 1.0f,  1.0f, -3.0f);
      
      glVertex3f(-1.0f, -1.0f, -3.0f);
      glVertex3f(-1.0f,  1.0f, -3.0f);
      
      glVertex3f(-1.0f,  1.0f, -3.0f);
      glVertex3f( 1.0f,  1.0f, -3.0f);
      
      glVertex3f( 1.0f,  1.0f, -3.0f);
      glVertex3f( 1.0f, -1.0f, -3.0f);
      
      glVertex3f( 1.0f, -1.0f, -3.0f);
      glVertex3f(-1.0f, -1.0f, -3.0f);
    glEnd();
    //glutSolidSphere(0.5f,10, 10);
  glPopMatrix();
  //drawing - weee
}