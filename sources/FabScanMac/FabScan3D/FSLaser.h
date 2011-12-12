//
//  FSLaser.h
//  FabScan3D
//
//  Created by Francis Engelmann on 6/30/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_LASER_H
#define FS_LASER_H

#include "FSHeaders.h"
#include "FSGeometry.h"
#include "FSObject.h"

class FSStepper;
class FSSerial2;

class FSLaser : public FSObject {
  private:
    FSPoint laserPointPosition;
    unsigned char servoAngle;
    unsigned char maxServoAngle;
    FSStepper* stepper;
    FSSerial2* serial;
    FSDirection direction;
    void turnToPosition(void);
    
  public:
    /* Constructor and Destructor */
    FSLaser(FSStepper* stepper, FSSerial2* serial, unsigned char maxPosition);
    ~FSLaser();
    
    /* Getters and Setters */
    void setLaserPointPosition(FSPoint p);
    FSPoint getLaserPointPosition(void);
    
    /* Methods */
    
    void turnOn();
    void turnOff();
    
    void turnToInitPosition();
    
    void turnToPosition(unsigned char position);
    
    /* Stepper functions */
    void turnNumberOfDegrees(FSFloat degrees);
    void setDirection(FSDirection direction);
    void toggleDirection();
    void enable(void);
    void disable(void);
    
    
    void draw(void);
};

#endif