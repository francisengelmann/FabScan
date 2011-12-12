//
//  FSStepper.h
//  FabScan3D
//
//  Created by Francis Engelmann on 9/8/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_STEPPER_H
#define FS_STEPPER_H

#include "FSHeaders.h"
#include "FSSerial2.h"

class FSStepper {
  private:
    FSFloat stepDegrees;                //the number of degrees of one step, defined by the resistors on the board
    FSDirection direction;
    FSSerial2* serial;
    FSUChar stepperID;                  //the id to address this stepper
    
  public:
    /* Constructor and Destructor */
    FSStepper(FSSerial2* serial, FSUChar stepperID);
    ~FSStepper(); 

    void selectStepper(unsigned char id);
    void turnNumberOfSteps(int steps);
    void turnNumberOfDegrees(FSFloat degrees);
    void setDirection(FSDirection direction);
    void toggleDirection();
    void enable(void);
    void disable(void);
};

#endif