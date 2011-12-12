//
//  FSTurnTable.cpp
//  FabScan3D
//
//  Created by Francis Engelmann on 8/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "FSTurnTable.h"

/* Constructor and Destructor */

FSTurnTable::FSTurnTable(FSStepper* stepper)
{
//  direction = FS_TURN_TABLE_DIRECTION_CCW;
//  stepDegrees = 0.0625f; //defined by the resitors on the polulu driver
  this->rotation.x = 0.0f;
  this->rotation.y = 0.0f;
  this->rotation.z = 0.0f;
  position.x = 0.0f;
  position.y = 0.0f;
  position.z = 7.5f;
  
  this->stepper = stepper;
}

FSTurnTable::~FSTurnTable()
{
  
}

void FSTurnTable::turnNumberOfDegrees(FSFloat degrees)
{
  int steps = (int)(degrees*200.0f*16.0f/360.0f);
  degrees = steps/200.0f/16.0f*360.0f;
  if(direction==FS_DIRECTION_CCW){
    rotation.y += degrees;
  }else if(direction==FS_DIRECTION_CCW){
    rotation.y -= degrees;
  }
  stepper->turnNumberOfDegrees(degrees);
  //printf("%s rotation %f \n",__PRETTY_FUNCTION__,rotation.y);
}

void FSTurnTable::setDirection(FSDirection direction)
{
  this->direction = direction;
  stepper->setDirection(direction);
}

void FSTurnTable::enable(void)
{
  //printf("%s \n",__PRETTY_FUNCTION__);
  stepper->enable();
  display=1;
}

void FSTurnTable::disable(void)
{
  //printf("%s \n",__PRETTY_FUNCTION__);
  stepper->disable();
  display=0;
}

void FSTurnTable::draw(void)
{
  glLineWidth(3.0f);

  glPushMatrix();
    glTranslated(position.x, position.y, position.z);
    glRotated(rotation.y, 0, 1, 0);
    glBegin(GL_LINES);
      glVertex3d(-7, 0, 0);
      glVertex3d( 7, 0, 0);
      glVertex3d( 0, 0, 7);
      glVertex3d( 0, 0,-7);
    glEnd();
  glPopMatrix();

  float xc = position.x;
  float yc = position.z;
  float z = 0;
  float rad = 7;

  //
  // draw a circle centered at (xc,yc) with radius rad
  //
  glBegin(GL_LINE_LOOP);
  for (int angle=0; angle < 360; angle=angle+5){
    float angle_radians = angle * (float)3.14159 / (float)180;
    float x = xc + rad * (float)cos(angle_radians);
    float y = yc + rad * (float)sin(angle_radians);
    glVertex3f(x,z,y);
  }
  glEnd();
}

