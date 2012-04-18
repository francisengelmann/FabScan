//
//  FSVision.cpp
//  FabScan3D
//
//  Created by Francis Engelmann on 7/4/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#include "FSVision.h"

/**
 * 
 * @param image with laser on, image with laser off, threshold value
 * @return RGB IplImage of binary result 
 */
IplImage* FSVision::subLaser(IplImage *frame, IplImage *laserFrame, FSFloat threshold){
  //printf("%s \n",__PRETTY_FUNCTION__);
    
  CvSize sz = cvSize(laserFrame->width, laserFrame->height);

  IplImage *bwNoLaser = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	IplImage *bwWithLaser = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	IplImage *subImage = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	IplImage *bitImage = cvCreateImage(sz, IPL_DEPTH_8U, 3);
  
  // the cvCvtColor function segfaults on windows.  Not sure why.
	cvCvtColor(frame, bwNoLaser,CV_RGB2GRAY);
	cvCvtColor(laserFrame, bwWithLaser,CV_RGB2GRAY);
  
  //cvSub(bwWithLaser, bwNoLaser, subImage);  
  //cvCvtColor(subImage, pframe,CV_GRAY2RGB);

  cvAbsDiff(bwNoLaser,bwWithLaser,subImage);
  cvThreshold(subImage, subImage, TRESHOLD_FOR_BW, 255, CV_THRESH_BINARY);

  cvCvtColor(subImage, bitImage, CV_GRAY2RGB);
  
  cvReleaseImage(&bwNoLaser);
  cvReleaseImage(&bwWithLaser);
  cvReleaseImage(&subImage);
  
  return bitImage;
}

void FSVision::addGreenLineTo(IplImage *pframe)
{  
  //artifical horizont
  cvLine(pframe, cvPoint(0,pframe->height*ORIGIN_Y), cvPoint(pframe->width,pframe->height*ORIGIN_Y), CV_RGB( 0,255,0 ),1,CV_AA,0);
  //cvLine(pframe, cvPoint(0,pframe->height*0.9), cvPoint(pframe->width,pframe->height*0.9), CV_RGB( 0,255,0 ),1,CV_AA,0);
  //cvLine(pframe, cvPoint(0,pframe->height*0.1), cvPoint(pframe->width,pframe->height*0.1), CV_RGB( 0,255,0 ),1,CV_AA,0);

  //two lines for center of frame
  cvLine(pframe, cvPoint(pframe->width*0.5f,0), cvPoint(pframe->width*0.5,pframe->height), CV_RGB( 0,255,0 ),1,CV_AA,0);
  cvLine(pframe, cvPoint(0,pframe->height*0.5f), cvPoint(pframe->width,pframe->height*0.5f), CV_RGB( 0,255,0 ),1,CV_AA,0);
  
  //line showing the upper limit where analyzing starts
  cvLine(pframe,
    cvPoint(0,pframe->height-LOWER_ANALYZING_FRAME_LIMIT),
    cvPoint(pframe->width,pframe->height-LOWER_ANALYZING_FRAME_LIMIT),
    CV_RGB( 255,255,0 ),1,CV_AA,0);

  //line showing the lower limit where analyzing stops    
  cvLine(pframe,
    cvPoint(0,UPPER_ANALYZING_FRAME_LIMIT),
    cvPoint(pframe->width,UPPER_ANALYZING_FRAME_LIMIT),
    CV_RGB(255,255,0), 1, CV_AA, 0);
}

FSPoint FSVision::convertCvPointToFSPoint(CvPoint cvPoint)
{
  CvSize cvImageSize = cvSize(CAM_IMAGE_WIDTH, CAM_IMAGE_HEIGHT); //1600 1200 is the resolution of the image from the camera
  FSSize fsImageSize = FSMakeSize(FRAME_WIDTH, FRAME_WIDTH*(CAM_IMAGE_HEIGHT/CAM_IMAGE_WIDTH), 0.0f);
  
  //here we define the origin of the cvImage, we place it in the middle of the frame and in the corner of the two perpendiculair planes
  CvPoint origin;
  origin.x = cvImageSize.width/2.0f;
  origin.y = cvImageSize.height*ORIGIN_Y;
  
  FSPoint fsPoint;
  
  //translate
  cvPoint.x -= origin.x;
  cvPoint.y -= origin.y;
  //scale
  fsPoint.x = cvPoint.x*fsImageSize.width/cvImageSize.width;
  fsPoint.y = -cvPoint.y*fsImageSize.height/cvImageSize.height;
  fsPoint.z=0.0f;
  
  return fsPoint;
}

CvPoint FSVision::convertFSPointToCvPoint(FSPoint fsPoint)
{
  CvSize cvImageSize = cvSize(CAM_IMAGE_WIDTH, CAM_IMAGE_HEIGHT);
  FSSize fsImageSize = FSMakeSize(FRAME_WIDTH, FRAME_WIDTH*(CAM_IMAGE_HEIGHT/CAM_IMAGE_WIDTH), 0.0f);
  CvPoint origin;
  origin.x = cvImageSize.width/2.0f;
  origin.y = cvImageSize.height*ORIGIN_Y;

  CvPoint cvPoint;

  cvPoint.x = fsPoint.x*cvImageSize.width/fsImageSize.width;
  cvPoint.y = -fsPoint.y*cvImageSize.height/fsImageSize.height;

  //translate
  cvPoint.x += origin.x;
  cvPoint.y += origin.y;

  return cvPoint;
}

void FSVision::drawLaserLineFromLaserToImageSeenFromCamera(FSLaser* laser, IplImage *image, FSCamera* camera)
{
  //FSPoint camPos = camera->getPosition();
  //FSPoint camRot = camera->getRotation();
  CvPoint cvLaserPoint = FSVision::convertFSPointToCvPoint(laser->getLaserPointPosition());
  FSFloat vertical = cvLaserPoint.x;
  FSFloat horizontal = FSVision::convertFSPointToCvPoint(FSMakePoint(0,0,0)).y;
  CvPoint p1 = cvPoint(vertical, 0); //top of laser line
  CvPoint p2 = cvPoint(vertical, horizontal); //bottom of laser line
  //CvPoint p3 = cvPoint(image->width, image->height);
  printf("%s %d %d \n",__PRETTY_FUNCTION__, image->height, image->width);
  cvLine(image, p1, p2, CV_RGB( 255,255,0 ),6,CV_AA,0);
  //cvLine(image, p2, p3, CV_RGB( 255,0,0 ),6,CV_AA,0);
}

void FSVision::putPointsFromFrameToCloud(IplImage* noLaserFrame, IplImage* laserFrame, FSPointCloud* pointCloud, FSLaser* laser, FSCamera* camera, FSTurnTable* turntable, int dpiVertical, FSFloat lowerLimit, FSFloat treshold)
{
    //extract laser line from the two images
    IplImage* laserLine = FSVision::subLaser(laserFrame,noLaserFrame,treshold);

    //calculate position of laser in cv frame
    FSPoint fsLaserLinePosition = laser->getLaserPointPosition();
    CvPoint cvLaserLinePosition = FSVision::convertFSPointToCvPoint(fsLaserLinePosition);
    
    FSFloat laserPos = cvLaserLinePosition.x;
    
    CvSize sz = cvSize(laserLine->width, laserLine->height);
    IplImage *bwImage = cvCreateImage(sz, IPL_DEPTH_8U, 1);

    cvCvtColor(laserLine, bwImage, CV_RGB2GRAY);
    
    cvReleaseImage(&laserLine);

    for( int y = UPPER_ANALYZING_FRAME_LIMIT; y < bwImage->height-LOWER_ANALYZING_FRAME_LIMIT; y+=dpiVertical ){ //no bear outside of these limits :) cuttin gof top and bottom of frame
      unsigned char* row = &CV_IMAGE_ELEM( bwImage, unsigned char, y, 0 );
      //why are we starting from the other side?
      
      //100 is the offset where we stop looking for a reflected laser, cos we might catch the non reflected
      for( int x = bwImage->width*bwImage->nChannels-1; x >= laserPos+ANALYZING_LASER_OFFSET; x -= bwImage->nChannels ){
        if (row[x] > 200){ //we have a white point in the black white image, so one edge laser line found
          //no we should continue to look for the other edge and then take the middle of those two points
          //to take the width of the laser line into account

          CvPoint cvNewPoint; //position of the reflected laser line on the image plane
          cvNewPoint.x = x;
          cvNewPoint.y = y;
                    
          FSPoint fsNewPoint = FSVision::convertCvPointToFSPoint(cvNewPoint); //convert to world coordinates
          
          FSLine2 l1 = computeLineFromPoints(camera->getPosition(), fsNewPoint);
          FSLine2 l2 = computeLineFromPoints(laser->getPosition(), laser->getLaserPointPosition());
  
          FSPoint i = computeIntersectionOfLines(l1, l2);
          
          fsNewPoint.x = i.x;
          fsNewPoint.z = i.z;
          
          //old stuff probably wrong
          //FSFloat angle = (laser->getRotation()).y;
          //fsNewPoint.z = (fsNewPoint.x - fsLaserLinePosition.x)/tan(angle*M_PI/180.0f);
          
          
          //At this point we know the depth=z. Now we need to consider the scaling depending on the depth.
          //First we move our point to a camera centered cartesion system.
          fsNewPoint.y -= (camera->getPosition()).y;
          fsNewPoint.y *= ((camera->getPosition()).z - fsNewPoint.z)/(camera->getPosition()).z;
          //Redo the translation to the box centered cartesion system.
          fsNewPoint.y += (camera->getPosition()).y;
          

          FSUChar r = CV_IMAGE_ELEM( noLaserFrame, unsigned char, y, 3*x+2);
          FSUChar g = CV_IMAGE_ELEM( noLaserFrame, unsigned char, y, 3*x+1);
          FSUChar b = CV_IMAGE_ELEM( noLaserFrame, unsigned char, y, 3*x+0);

          //printf("COLOR %u %u %u \n",r,g,b);
          fsNewPoint.color = FSMakeColor(r, g, b);
          
          //turning new point according to current angle of turntable
          //translate coordinate system to the middle of the turntable
          fsNewPoint.z -= 7.5f; //7cm radius of turntbale plus 5mm offset from back plane
          FSPoint alphaDelta = turntable->getRotation();
          //printf("%s %f \n", __PRETTY_FUNCTION__, alphaDelta.y);
          FSFloat alphaOld = (float)atan(fsNewPoint.z/fsNewPoint.x);
          //printf("%s %f \n",__PRETTY_FUNCTION__,alphaOld);
          FSFloat alphaNew = alphaOld+alphaDelta.y*(M_PI/180.0f);
          FSFloat hypotenuse = (float)sqrt(fsNewPoint.x*fsNewPoint.x + fsNewPoint.z*fsNewPoint.z);
          
          if(fsNewPoint.z < 0 && fsNewPoint.x < 0){
            alphaNew += M_PI;
          }else if(fsNewPoint.z > 0 && fsNewPoint.x < 0){
            alphaNew -= M_PI;
          }
          fsNewPoint.z = (float)sin(alphaNew)*hypotenuse;
          fsNewPoint.x = (float)cos(alphaNew)*hypotenuse;
          
          if(fsNewPoint.y>lowerLimit && hypotenuse < 7){ //eliminate points from the grounds, that are not part of the model
            pointCloud->addPoint(fsNewPoint);
          }
          break;
        }
      }
    }
  cvReleaseImage(&bwImage);
}