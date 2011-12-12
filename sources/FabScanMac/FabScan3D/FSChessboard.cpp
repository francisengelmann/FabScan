//
//  FSChessboard.cpp
//  FabScan3D
//
//  Created by Francis Engelmann on 7/3/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#include "FSChessboard.h"

// --------------------------------------------------
#pragma mark -
#pragma mark Constrcutors and Destructors
// --------------------------------------------------

FSChessboard::FSChessboard()
{
  position = FSMakePoint(0.0f, 0.0f, 0.0f);
}

FSChessboard::~FSChessboard()
{
}

// --------------------------------------------------
#pragma mark -
#pragma mark Getters and Setters
// --------------------------------------------------

// --------------------------------------------------
#pragma mark -
#pragma mark draw
// --------------------------------------------------

void FSChessboard::draw(void)
{
glPushMatrix();
  glTranslated(0.0, 0.0, 0.1);
  
  glColor4f(1.0f, 1.0f, 1.0f,1.0f);
  /*glBegin(GL_QUADS);
     glVertex3f(0,0,0);
     glVertex3f(1,0,0);
     glVertex3f(1,10,0);
     glVertex3f(0,10,0);
  glEnd();
  
  glBegin(GL_QUADS);
     glVertex3f(1,0,0);
     glVertex3f(15,0,0);
     glVertex3f(15,1,0);
     glVertex3f(1,1,0);
  glEnd();*/
      
  for(int x=1;x<15;x++){
    for(int y=1;y<10;y++){
      if((x+y)%2){
          glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      }else{
          glColor4f(0.0f, 0.0f, 0.0f,1.0f);
      }
      glBegin(GL_QUADS);
        glVertex3f(x+0,y+0,0);
        glVertex3f(x+0,y+1,0);
        glVertex3f(x+1,y+1,0);
        glVertex3f(x+1,y+0,0);
      glEnd();
    }
  }
  glPopMatrix();
}