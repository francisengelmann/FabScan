//
//  FSTurnTable.h
//  FabScan3D
//
//  Created by Francis Engelmann on 8/25/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_TURN_TABLE_H
#define FS_TURN_TABLE_H

#include "FSHeaders.h"

#include "FSObject.h"
#include "FSStepper.h"

class FSTurnTable : public FSObject {
  private:
    FSDirection direction;
    FSStepper* stepper;
    
  public:
    /* Constructor and Destructor */
    FSTurnTable(FSStepper* stepper);
    ~FSTurnTable();

    //void selectStepper(unsigned char id);
    
    void turnNumberOfDegrees(FSFloat degrees);
    void setDirection(FSDirection direction);
    void enable(void);
    void disable(void);
    
    void draw(void);
};

#endif
