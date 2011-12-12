//
//  FSChessboardCalibration.cpp
//  FabScan3D
//
//  Created by Francis Engelmann on 7/8/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#include "FSChessboardCalibration.h"

#include <iostream>
using namespace std;


FSChessboardCalibration::FSChessboardCalibration(const char* resPath)
{
  this->resPath = resPath;
  printf("%s rePath is: %s \n",__PRETTY_FUNCTION__,this->resPath);
  /**
   * number of board shots for calibration
   * minimum 2 for camera calibration
   */
  n_boards = 10;
  board_dt = 20;
  board_w = 13;
  board_h = 8;
  //board_w = 3;
  //board_h = 5;
  board_n = board_w * board_h;
  board_sz = cvSize( board_w, board_h );
  
  image_points = cvCreateMat(n_boards*board_n,2,CV_32FC1);
  object_points = cvCreateMat(n_boards*board_n,3,CV_32FC1);
  point_counts = cvCreateMat(n_boards,1,CV_32SC1);
  intrinsic_matrix = cvCreateMat(3,3,CV_32FC1);
  distortion_coeffs = cvCreateMat(5,1,CV_32FC1);
  
  corners = new CvPoint2D32f[ board_n ];
  successes = 0;
  step = 0;
  frame = 0;
}

FSChessboardCalibration::~FSChessboardCalibration()
{
  cvReleaseMat(&object_points);
  cvReleaseMat(&image_points);
  cvReleaseMat(&point_counts);
}

CvMat* FSChessboardCalibration::getInstrinsicMatrix(void)
{
  return intrinsic_matrix;
}

CvMat* FSChessboardCalibration::getDistortionCoeffs(void)
{
  return distortion_coeffs;
}


int FSChessboardCalibration::needMoreBoardsForCalibration(void)
{
  return successes < n_boards;
}

FSPoint FSChessboardCalibration::findExtrinsics(IplImage* image)
{
  successes = 0;
  this->findBoardInFrame(image);
  successes = n_boards;
  FSPoint trans = this->calibrateCamera();
  return trans;
}

int FSChessboardCalibration::autoCalibrate()
{
  printf("%s rePath is: %s \n",__PRETTY_FUNCTION__,this->resPath);

  successes=0;
  for(int i=1;i<=n_boards;i++){
    char* name;
    asprintf(&name,"%s/calib%d.jpg",this->resPath,i);
    printf("%s loading %s \n",__PRETTY_FUNCTION__,name);
    IplImage* frame = cvLoadImage(name);
    this->findBoardInFrame(frame);
  }
  return (successes==n_boards);
}

int FSChessboardCalibration::findBoardInFrame(IplImage* image)
{
  IplImage *gray_image = cvCreateImage(cvGetSize(image),8,1);
  //Find chessboard corners:
  int found = cvFindChessboardCorners(image, board_sz, corners, &corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
  //Get Subpixel accuracy on those corners
  cvCvtColor(image, gray_image, CV_BGR2GRAY);
  cvFindCornerSubPix(gray_image, corners, corner_count, cvSize(11,11),cvSize(-1,-1), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
  
  //Draw it
  cvDrawChessboardCorners(image, board_sz, corners, corner_count, found);
  
  if( corner_count == board_n ){
    step = successes*board_n;
    for(int i=step, j=0; j<board_n; ++i,++j ){
      CV_MAT_ELEM(*image_points, float,i,0) = corners[j].x;
      CV_MAT_ELEM(*image_points, float,i,1) = corners[j].y;
      CV_MAT_ELEM(*object_points,float,i,0) = j/board_w;
      CV_MAT_ELEM(*object_points,float,i,1) = j%board_w;
      CV_MAT_ELEM(*object_points,float,i,2) = 0.0f;
    }
    CV_MAT_ELEM(*point_counts, int,successes,0) = board_n;
    successes++;
  }
  //needed by calibrateCamera
  image_sz = cvGetSize(image);
  
  if (corner_count == board_n) return successes; //if a complete board was detected
  return 0;
}

IplImage* FSChessboardCalibration::findBoardInFrame2(IplImage* image, CvMat* intrinsic, CvMat* distortion, CvMat* rot, CvMat* trans)
{
  int successes = 0;
  
  CvMat* rotat = cvCreateMat(1,3,CV_32F);
  CvMat* transit = cvCreateMat(1,3,CV_32F);
  
  CvMat* image_points = cvCreateMat(n_boards*board_n,2,CV_32FC1);
  CvMat* object_points = cvCreateMat(n_boards*board_n,3,CV_32FC1);
  CvMat* point_counts = cvCreateMat(n_boards,1,CV_32SC1);
  
  IplImage *gray_image = cvCreateImage(cvGetSize(image),8,1);
  //Find chessboard corners:
  int found = cvFindChessboardCorners(image, board_sz, corners, &corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
  //Get Subpixel accuracy on those corners
  cvCvtColor(image, gray_image, CV_BGR2GRAY);
  cvFindCornerSubPix(gray_image, corners, corner_count, cvSize(11,11),cvSize(-1,-1), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
  
  //Draw it - no not needed
  cvDrawChessboardCorners(image, board_sz, corners, corner_count, found);
  
  if( corner_count == board_n ){
    step = successes*board_n;
    for(int i=step, j=0; j<board_n; ++i,++j ){
      CV_MAT_ELEM(*image_points, float,i,0) = corners[j].x;
      CV_MAT_ELEM(*image_points, float,i,1) = corners[j].y;
      CV_MAT_ELEM(*object_points,float,i,0) = j/board_w;
      CV_MAT_ELEM(*object_points,float,i,1) = j%board_w;
      CV_MAT_ELEM(*object_points,float,i,2) = 0.0f;
    }
    CV_MAT_ELEM(*point_counts, int,successes,0) = board_n;
    successes++;
  }
  
  cvFindExtrinsicCameraParams2(object_points, image_points, intrinsic_matrix, distortion_coeffs, rotat, transit);
  
  cout << CV_MAT_ELEM(*transit, float, 0, 0) << " ";
  cout << CV_MAT_ELEM(*transit, float, 0, 1) << " ";
  cout << CV_MAT_ELEM(*transit, float, 0, 2) << endl;
  
  //RQDecomp3x3
  
  return image;
  
  //float tranV[3] = {0, 0, 0 };
  //float rotV[3] = {0, 0, 0};
  //float rotW2C[9] = {1,0,0, 0,1,0 ,0,0,1};
  
  //CvMat transW2C = cvMat(3, 1, CV_32FC1, tranV);
  //CvMat rotateTmp = cvMat(3, 1, CV_32FC1, rotV);
  //CvMat rotateW2C = cvMat(3, 3, CV_32FC1, rotW2C);
  
  //cvFindExtrinsicCameraParams2(object_points, image_points, intrinsic_matrix, distortion_coeffs, &rotateTmp, &transW2C);
  
  //cvRodrigues2(&rotateTmp, &rotateW2C);*/

  //cout << corners[0] << " " << corners[0].y << endl;

/*    float *data = trans->data.fl;
    for (int r = 0; r < trans->rows; ++r){
        for (int c = 0; c < trans->cols; ++c){ printf("%+1.9lf\t", data[r*trans->cols+c]);}
        printf("\n");
    }
    
    data = rot->data.fl;
    for (int r = 0; r < rot->rows; ++r){
        for (int c = 0; c < rot->cols; ++c){ printf("%+1.9f\t", data[r*rot->cols+c]);}
        printf("\n");
    }*/
  /*CvPoint c1 = cvPoint(corners[0].x, corners[0].y);
  CvPoint c2 = cvPoint(corners[board_w-1].x, corners[board_w-1].y);
  CvPoint c3 = cvPoint(corners[board_w*board_h-1].x, corners[board_w*board_h-1].y);
  CvPoint c4 = cvPoint(corners[board_w*board_h-board_w].x, corners[board_w*board_h-board_w].y);

  CvPoint m1 = cvPoint((c1.x+c2.x)/2,(c1.y+c2.y)/2);
  CvPoint m2 = cvPoint((c2.x+c3.x)/2,(c2.y+c3.y)/2);
  CvPoint m3 = cvPoint((c3.x+c4.x)/2,(c3.y+c4.y)/2);
  CvPoint m4 = cvPoint((c4.x+c1.x)/2,(c4.y+c1.y)/2);

  cvLine(image,c1,c2,CV_RGB( 255,0,0 ),3,CV_AA,0);
  cvLine(image,c2,c3,CV_RGB( 255,0,0 ),3,CV_AA,0);
  cvLine(image,c3,c4,CV_RGB( 255,0,0 ),3,CV_AA,0);
  cvLine(image,c4,c1,CV_RGB( 255,0,0 ),3,CV_AA,0);

  cvLine(image,c1,c3,CV_RGB( 0,255,0 ),3,CV_AA,0);
  cvLine(image,c2,c4,CV_RGB( 0,255,0 ),3,CV_AA,0);*/
  
  //-------------------------------------------------------------
  
  //CvMat* imagePoints = cvCreateMat(4,2,CV_32FC1);
  //CvMat* objectPoints = cvCreateMat(4,3,CV_32FC1);
  
  /*CV_MAT_ELEM(*imagePoints, float,0,0) = c1.x;
  CV_MAT_ELEM(*imagePoints, float,0,1) = c1.y;
  CV_MAT_ELEM(*imagePoints, float,1,0) = c2.x;
  CV_MAT_ELEM(*imagePoints, float,1,1) = c2.y;
  CV_MAT_ELEM(*imagePoints, float,2,0) = c3.x;
  CV_MAT_ELEM(*imagePoints, float,2,1) = c3.y;
  CV_MAT_ELEM(*imagePoints, float,3,0) = c4.x;
  CV_MAT_ELEM(*imagePoints, float,3,1) = c4.y;
  
  CV_MAT_ELEM(*objectPoints,float,0,0) = 0.0f;
  CV_MAT_ELEM(*objectPoints,float,0,1) = 0.0f;
  CV_MAT_ELEM(*objectPoints,float,0,2) = 0.0f;
  
  CV_MAT_ELEM(*objectPoints,float,1,0) = 1.0f;
  CV_MAT_ELEM(*objectPoints,float,1,1) = 0.0f;
  CV_MAT_ELEM(*objectPoints,float,1,2) = 0.0f;
  
  CV_MAT_ELEM(*objectPoints,float,2,0) = 1.0f;
  CV_MAT_ELEM(*objectPoints,float,2,1) = 1.0f;
  CV_MAT_ELEM(*objectPoints,float,2,2) = 0.0f;
  
  CV_MAT_ELEM(*objectPoints,float,3,0) = 0.0f;
  CV_MAT_ELEM(*objectPoints,float,3,1) = 1.0f;
  CV_MAT_ELEM(*objectPoints,float,3,2) = 0.0f;*/
  
  /*CvPoint m6 = cvPoint(300,300);
  CvPoint m7 = cvPoint(300,
    300+CV_MAT_ELEM(*trans,float,0,2)*30
    );
  cvLine(image,m6,m7,CV_RGB( 255,255,0 ),3,CV_AA,0);*/

}

FSPoint FSChessboardCalibration::calibrateCamera()
{
  object_points2 = cvCreateMat(successes*board_n,3,CV_32FC1);
  image_points2	= cvCreateMat(successes*board_n,2,CV_32FC1);
  point_counts2	= cvCreateMat(successes,1,CV_32SC1);
    
  //TRANSFER THE POINTS INTO THE CORRECT SIZE MATRICES
  for(int i = 0; i<successes*board_n; ++i) {
    CV_MAT_ELEM( *image_points2, float, i, 0) = CV_MAT_ELEM( *image_points, float, i, 0);
    CV_MAT_ELEM( *image_points2, float,i,1) = CV_MAT_ELEM( *image_points, float, i, 1);
    CV_MAT_ELEM( *object_points2, float, i, 0) = CV_MAT_ELEM( *object_points, float, i, 0) ;
    CV_MAT_ELEM( *object_points2, float, i, 1) = CV_MAT_ELEM( *object_points, float, i, 1) ;
    CV_MAT_ELEM( *object_points2, float, i, 2) = CV_MAT_ELEM( *object_points, float, i, 2) ;
  }

  for(int i=0; i<successes; ++i){ //These are all the same number
    CV_MAT_ELEM( *point_counts2, int, i, 0) = CV_MAT_ELEM( *point_counts, int, i, 0);
  }
  // At this point we have all of the chessboard corners we need.
  // Initialize the intrinsic matrix such that the two focal
  // lengths have a ratio of 1.0 //
  CV_MAT_ELEM(*intrinsic_matrix, float, 0, 0 ) = 1.0f;
  CV_MAT_ELEM(*intrinsic_matrix, float, 1, 1 ) = 1.0f;
  
  CvMat* rotation_vectors    = cvCreateMat(successes, 3, CV_32FC1);
  CvMat* translation_vectors = cvCreateMat(successes, 3, CV_32FC1);
  
  //CALIBRATE THE CAMERA!
  cvCalibrateCamera2(object_points2, image_points2, point_counts2,image_sz, intrinsic_matrix, distortion_coeffs, rotation_vectors, translation_vectors,0 //CV_CALIB_FIX_ASPECT_RATIO
  );
  
  //Not yet used, see BYO3D
  //CvMat* R = cvCreateMat(3, 3, CV_32FC1);
  //CvMat* r = cvCreateMat(1, 3, CV_32FC1);
  
  /*
  cout << "intrinsicMatrix" << endl;
  cout << CV_MAT_ELEM(*intrinsic_matrix, float, 0, 0 ) << " ";
  cout << CV_MAT_ELEM(*intrinsic_matrix, float, 0, 1 ) << " ";
  cout << CV_MAT_ELEM(*intrinsic_matrix, float, 0, 2 ) << endl;
  cout << CV_MAT_ELEM(*intrinsic_matrix, float, 1, 0 ) << " ";
  cout << CV_MAT_ELEM(*intrinsic_matrix, float, 1, 1 ) << " ";
  cout << CV_MAT_ELEM(*intrinsic_matrix, float, 1, 2 ) << endl;
  cout << CV_MAT_ELEM(*intrinsic_matrix, float, 2, 0 ) << " ";
  cout << CV_MAT_ELEM(*intrinsic_matrix, float, 2, 1 ) << " ";
  cout << CV_MAT_ELEM(*intrinsic_matrix, float, 2, 2 ) << endl; 

  cout << "calibrate" << endl;
  
  for(int i=0;i<n_boards;i++){
    cout  << CV_MAT_ELEM( *translation_vectors, float, i, 0) << " "
        << CV_MAT_ELEM( *translation_vectors, float, i, 1) << " "
        << CV_MAT_ELEM( *translation_vectors, float, i, 2) << endl;
  
  }
      */
  FSPoint camPosition;
  camPosition.x = -CV_MAT_ELEM( *translation_vectors, float, 0, 0);
  camPosition.y = CV_MAT_ELEM( *translation_vectors, float, 0, 1);
  camPosition.z = CV_MAT_ELEM( *translation_vectors, float, 0, 2);
  
  // SAVE THE INTRINSICS AND DISTORTIONS
  
  char* intrinsicsPath;
  asprintf(&intrinsicsPath,"%s/Intrinsics.xml",resPath);
  char* distortionPath;
  asprintf(&distortionPath,"%s/Distortion.xml",resPath);  
  
  cvSave(intrinsicsPath, intrinsic_matrix);
  cvSave(distortionPath, distortion_coeffs);
    
  cvReleaseMat(&object_points2);
  cvReleaseMat(&image_points2);
  cvReleaseMat(&point_counts2);
  
  return camPosition;
}