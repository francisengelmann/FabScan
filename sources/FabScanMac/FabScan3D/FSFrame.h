//
//  FSFrame.h
//  FabScan
//
//  Created by Francis Engelmann on 7/3/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_FRAME_H
#define FS_FRAME_H

#include "FSHeaders.h"
#include "FSObject.h"

using namespace std;

class FSFrame : public FSObject {
  private:
    IplImage* frame;
    FSSize size;
    float w; //width
    float h; //height
  public:
    /* Constructors and Destructors */
    FSFrame();
    FSFrame(FSPoint position);
    ~FSFrame();
    
    /* Getters and Setters */
    void setFrame(IplImage* frame);
    IplImage* getFrame(void);
    
    void setSize(FSSize size);
    FSSize getSize(void);
    
    /* other methods */
    void draw(void);
};

#endif