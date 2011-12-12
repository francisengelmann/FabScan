/*
 *  FSWorldCamera.cpp
 *
 *  Created by Francis Engelmann on 3/19/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
 *
 */

#include "FSWorldCamera.h"

// --------------------------------------------------
#pragma mark -
#pragma mark Constructors and Destructors
// --------------------------------------------------

FSWorldCamera::FSWorldCamera()
{
  printf("FSWorldCamera constructor start \n");

  //position and target currently not used
  /*position = FSMakePoint(0, 1,1);*/
  target = FSMakePoint(0, 4, 7.5f);
  upVector = FSMakePoint(0, 1, 0);
  
  angleX = (float)M_PI/2.0f;
  angleY = 1;
  distance = 32;
  printf("FSWorldCamera constructor end \n");
}

FSWorldCamera::~FSWorldCamera(){
  
}

// --------------------------------------------------
#pragma mark -
#pragma mark Getters and Setters
// --------------------------------------------------

void FSWorldCamera::setPosition(FSPoint position){
  this->position = position;
}
    
FSPoint FSWorldCamera::getPosition(void){
  return position;
}

void FSWorldCamera::setTarget(FSPoint target){
  this->target = target;
}
    
FSPoint FSWorldCamera::getTarget(void){
  return target;
}


void FSWorldCamera::setUpVector(FSPoint upVector){
  this->upVector = upVector;
}

FSPoint FSWorldCamera::getUpVector(void){
  return upVector;
}

void FSWorldCamera::setDistance(FSFloat distance){
  if(distance>3){
    this->distance = distance;
  }else{
    this->distance = 3;  
  }
}
    
FSFloat FSWorldCamera::getDistance(void){
  return distance;
}

void FSWorldCamera::setAngleX(FSFloat angleX){
  if(angleX>M_PI){ angleX-=2*(FSFloat)M_PI;}
  if(angleX<-M_PI){ angleX+=2*(FSFloat)M_PI;}
  this->angleX = angleX;
}

FSFloat FSWorldCamera::getAngleX(void){
  return angleX;
}

void FSWorldCamera::setAngleY(FSFloat angleY){
  if(angleY>M_PI-0.01){ angleY=(FSFloat)(M_PI-0.01);}
  if(angleY<0.01){ angleY=(float)0.01; }
  this->angleY = angleY;
}

    
FSFloat FSWorldCamera::getAngleY(void){
  return angleY;
}

// --------------------------------------------------
#pragma mark -
#pragma mark Methods
// --------------------------------------------------

void FSWorldCamera::activate(void){
  //gluLookAt(position->getX(), position->getY(), position->getZ(),
    //        target->getX(), target->getY(), target->getZ(),
     //        0.0, 1.0, 0.0); // cam-pos, target-pos, cam-up-vector
     //printf("%s %f %f %f \n",__PRETTY_FUNCTION__,
     // -distance*sin(angleY)*cos(angleX)+target.x,
     // distance*cos(angleY)+target.y,
     // distance*sin(angleY)*sin(angleX)+target.z);
  gluLookAt(
    -distance*sin(angleY)*cos(angleX)+target.x,
    distance*cos(angleY)+target.y,
    distance*sin(angleY)*sin(angleX)+target.z,
    target.x, target.y, target.z,
    upVector.x,
    upVector.y,
    upVector.z); // cam-pos, target-pos, cam-up-vector*/
  //gluLookAt(1, 1, 6, 0, 0, 0, 0.0, 1.0, 0.0); // cam-pos, target-pos, cam-up-vector
  //printf("= activate = \n");
}