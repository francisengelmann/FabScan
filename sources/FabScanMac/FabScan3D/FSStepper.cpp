//
//  FSStepper.cpp
//  FabScan3D
//
//  Created by Francis Engelmann on 9/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "FSStepper.h"

FSStepper::FSStepper(FSSerial2* serial, FSUChar stepperID)
{
  this->serial = serial;
  this->stepperID = stepperID;
  stepDegrees = 0.0625f; //defined by the resitors on the polulu driver
}

FSStepper::~FSStepper()
{}

void FSStepper::selectStepper(unsigned char id)
{
    serial->writeByte(MC_SELECT_STEPPER);
    serial->writeByte(id);
}

void FSStepper::turnNumberOfSteps(int steps)
{
  selectStepper(stepperID);
  int s = steps;
  for(int i=0; i<=steps/256; i++){
    //printf("%s steps to be performed %d \n",__PRETTY_FUNCTION__, s);
    serial->writeByte(MC_PERFORM_STEP);
    usleep(1000);
    if(s<256){
      serial->writeByte(s%256);
    }else{
      serial->writeByte(255);
      s-=256;
    }
    usleep(100000);
  }
}

void FSStepper::turnNumberOfDegrees(FSFloat degrees)
{
  //one turn around of the table is 200*16 steps, 16 'cos of micro-stepping
  int steps = (int)(degrees*200.0f*16.0f/360.0f);
  //printf("%s steps %f \n",__PRETTY_FUNCTION__,degrees*200.0f*16.0f/360.0f);

  this->turnNumberOfSteps(steps);
  //printf("%s rotation %f \n",__PRETTY_FUNCTION__,rotation.y);
}

void FSStepper::setDirection(FSDirection direction)
{
  this->direction = direction;
  selectStepper(stepperID);
  if(direction == FS_DIRECTION_CW){
    serial->writeByte(MC_SET_DIRECTION_CW);  
  }else if(direction == FS_DIRECTION_CCW){
    serial->writeByte(MC_SET_DIRECTION_CCW);
  }
}

void FSStepper::toggleDirection(){
  if(direction == FS_DIRECTION_CW){
    this->setDirection(FS_DIRECTION_CCW);
  }else{
    this->setDirection(FS_DIRECTION_CW);    
  }
}


void FSStepper::enable(void)
{
  selectStepper(stepperID);
  serial->writeByte(MC_TURN_STEPPER_ON);
  usleep(10000);
}

void FSStepper::disable(void)
{
  selectStepper(stepperID);
  serial->writeByte(MC_TURN_STEPPER_OFF);
  usleep(10000);
}