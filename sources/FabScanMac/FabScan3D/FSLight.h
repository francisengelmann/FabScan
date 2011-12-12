//
//  FSLight.h
//  FabScan
//
//  Created by Francis Engelmann on 9/9/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_LIGHT_H
#define FS_LIGHT_H

#include "FSHeaders.h"
#include "FSSerial2.h"

class FSLight {
  private:
    FSSerial2* serial;
    
  public:
    FSLight(FSSerial2* serial);
    ~FSLight();
  
  void turnOn(unsigned char intensity);
  void turnOn();
  void turnOff();

};

#endif