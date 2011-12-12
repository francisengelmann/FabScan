//
//  FSObject.h
//  FabScan
//
//  Created by Francis Engelmann on 7/2/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_OBJECT_H
#define FS_OBJECT_H

#include "FSGeometry.h"

class FSObject {
  protected:
    FSPoint rotation;
    FSPoint position;
    int display; //wether the object should be displayed or not
    
  public:
    /* Constructor and Destructor */
    FSObject();
    ~FSObject();
    
    /* Getters and Setters */
    void setPosition(FSPoint);
    FSPoint getPosition(void);
    
    void setRotation(FSPoint);
    FSPoint getRotation(void);
    
    unsigned char shouldDisplay(void);
    void setShouldDisplay(int d);

    virtual void draw()=0;
};

#endif