//
//  FSLaser.cpp
//  FabScan3D
//
//  Created by Francis Engelmann on 6/30/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "FSLaser.h"
#include "FSStepper.h"
#include "FSSerial2.h"

// ---------------------------------------------
#pragma mark -
#pragma mark Constructor and Destructor
// ---------------------------------------------

FSLaser::FSLaser(FSStepper* stepper, FSSerial2* serial, unsigned char maxServoAngle)
{
  this->serial = serial;
  display = 0;
  this->stepper = stepper;
  //this->maxServoAngle = maxServoAngle;
  //servoAngle = 0;
  rotation = FSMakePoint(0.0f, 0.0f, 0.0f);
  laserPointPosition = FSMakePoint(14.0f, 0.0f, 0.0f);
  //this->turnToPosition();
}

FSLaser::~FSLaser()
{
  printf("%s destructor\n",__PRETTY_FUNCTION__);
}

// ---------------------------------------------
#pragma mark -
#pragma mark Setters and Getters
// ---------------------------------------------

void FSLaser::setLaserPointPosition(FSPoint p){
  laserPointPosition = p;
}

FSPoint FSLaser::getLaserPointPosition(void){
  return laserPointPosition;
}

/* moves the laser to the position where it was when the program was start, should be zero when the laser was placed correctly in the beginning */
void FSLaser::turnToInitPosition()
{
  enable();
  usleep(5000);
  
  if(rotation.y > 0){
    setDirection(FS_DIRECTION_CW);
    usleep(5000);
    turnNumberOfDegrees(rotation.y);
  }else{
    setDirection(FS_DIRECTION_CCW);
    usleep(5000);
    turnNumberOfDegrees(-rotation.y);
  }
  usleep(5000);
  disable();
  usleep(5000);
  turnOff();
  usleep(5000);

}

// ---------------------------------------------
#pragma mark -
#pragma mark Stepper functions
// ---------------------------------------------

void FSLaser::turnNumberOfDegrees(FSFloat degrees)
{
  int steps = (int)(degrees*200.0f*16.0f/360.0f);
  degrees = steps/200.0f/16.0f*360.0f;
  if(direction==FS_DIRECTION_CCW){
    rotation.y += degrees;
  }else if(direction==FS_DIRECTION_CW){
    rotation.y -= degrees;
  }
  stepper->turnNumberOfSteps(steps);
  
  laserPointPosition.x = (float)tan((-rotation.y)*(M_PI/180.0f))*position.z+position.x;
  laserPointPosition.y = this->position.y;
  laserPointPosition.z = 0.0f;
  //printf("%s rotation %f \n",__PRETTY_FUNCTION__,rotation.y);
}

void FSLaser::setDirection(FSDirection direction)
{
  this->direction = direction;
  stepper->setDirection(direction);
}

void FSLaser::toggleDirection()
{
  if(direction == FS_DIRECTION_CW){
    this->setDirection(FS_DIRECTION_CCW);
  }else{
    this->setDirection(FS_DIRECTION_CW);    
  }
}


void FSLaser::turnOn(void)
{
  //printf("%s \n",__PRETTY_FUNCTION__);
  serial->writeByte(MC_TURN_LASER_ON);
  display=1;
}

void FSLaser::turnOff(void)
{
  //printf("%s \n",__PRETTY_FUNCTION__);
  serial->writeByte(MC_TURN_LASER_OFF);
  display=0;
}

void FSLaser::enable(void)
{
  stepper->enable();
}

void FSLaser::disable(void)
{
  stepper->disable();
}

// ---------------------------------------------
#pragma mark -
#pragma mark Private helper methods
// ---------------------------------------------

/* obsolete */
void FSLaser::turnToPosition()
{
  printf("%s obsolete function",__PRETTY_FUNCTION__);
  serial->writeByte(MC_ROTATE_LASER); //make the number bigger to move to the left
  serial->writeByte(servoAngle+48); //make the number bigger to move to the left
}

void FSLaser::draw(void)
{
  
  /*FSPoint p = FSMakePoint(laserPointPosition.x, laserPointPosition.y, laserPointPosition.z);
  
  glPushMatrix();
      glTranslated(p.x, p.y, p.z);
      //glutSolidSphere(0.5f,10.0f, 10.0f);
  glPopMatrix();
  */
  
  glPushMatrix();
    glTranslated(this->position.x, this->position.y, this->position.z);
    glRotated(this->rotation.x,1,0,0);
    glRotated(this->rotation.y,0,1,0);
    glRotated(this->rotation.z,0,0,1);
    
    GLUquadric *myQuad = gluNewQuadric();
    GLUquadric *myQuad2 = gluNewQuadric();
    glPushMatrix();
    //glRotated(servoAngle, 0, 1, 0);
    //glRotated(133, 0, 1, 0);
    glRotated(180, 0, 1, 0);
    
    //Laser Plane
    glColor3f(1.0f, 0.0f, 0.0f);    
    glBegin(GL_LINE_LOOP);
      //glVertex3f(0.0, 0.0f, 0.0f);
      glVertex3f(0.0f, position.y, 4.0f);
      glVertex3f(0.0f, -position.y, 4.0f);
      glVertex3f(0.0f, -position.y, position.z/(float)cos(rotation.y*M_PI/180.0f));
      glVertex3f(0.0f, position.y,  position.z/(float)cos(rotation.y*M_PI/180.0f));
    glEnd();
    
    glEnable(GL_BLEND); //Enable blending.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.
    glColor4f( 1.0f, 0.0f, 0.0f,0.2f);
    
    glBegin(GL_TRIANGLES);
      glVertex3f(0.0f, 0.0f, 4.0f);
      glVertex3f(0.0f, -position.y, position.z/(float)cos(rotation.y*M_PI/180.0f));
      glVertex3f(0.0f, position.y,  position.z/(float)cos(rotation.y*M_PI/180.0f));
    glEnd();
    
    glDisable(GL_BLEND);
    //end laser plane

    glColor3f(0.4f, 0.4f, 0.4f);
    gluCylinder( myQuad, 1.0f, 1.0f, 4, 8, 1);
    glTranslatef(0, 0, 1.2f);
    gluDisk(myQuad2, 0, 0.1f, 15, 1);
  
  glPopMatrix();
  gluDeleteQuadric(myQuad);
  gluDeleteQuadric(myQuad2);
    
  glPopMatrix();
}