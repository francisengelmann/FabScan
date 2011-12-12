//
//  FSBox.h
//  FabScan3D
//
//  Created by Francis Engelmann on 7/17/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_BOX_H
#define FS_BOX_H

#include "FSHeaders.h"
#include "FSObject.h"

using namespace std;

class FSBox : public FSObject {
  private:
    float w; //width
    float h; //height
  public:
    /* Constructors and Destructors */
    FSBox();
    ~FSBox();
    
    /* Getters and Setters */
    
    /* other methods */
    void draw(void);
};

#endif