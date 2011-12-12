//
//  FSChessboard.h
//  FabScan3D
//
//  Created by Francis Engelmann on 7/3/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_CHESSBOARD_H
#define FS_CHESSBOARD_H

#include "FSHeaders.h"
#include "FSObject.h"

using namespace std;

class FSChessboard : public FSObject {
  private:
    float w; //width
    float h; //height
  public:
    /* Constructors and Destructors */
    FSChessboard();
    ~FSChessboard();
    
    /* Getters and Setters */
    
    /* other methods */
    void draw(void);
};

#endif