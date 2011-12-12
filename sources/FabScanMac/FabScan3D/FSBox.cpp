//
//  FSBox.cpp
//  FabScan3D
//
//  Created by Francis Engelmann on 7/17/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#include "FSBox.h"

FSBox::FSBox()
{
  position = FSMakePoint(0.0f, 0.0f, 0.0f);
}

FSBox::~FSBox()
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

void FSBox::draw(void)
{
  float width = 48.0f;
  float depth = 29.0f;
  glPushMatrix();
  glTranslated(-width/2.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);

    glBegin(GL_LINES);
      glColor3f( 1.0f, 0.7f, 0.0f);
      for(int i=0;i<=width;i++){
        glVertex3f(i, 0.0f, 0.0f);
        glVertex3f(i, 0.0f, depth);
      }
      for(int i=0;i<=depth;i++){
        glVertex3f(0.0f, 0.0f, i);
        glVertex3f(width, 0.0f, i);
      }
    glEnd();
    
    
    glEnable(GL_BLEND); //Enable blending.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.
    glColor4f( 1.0f, 1.0f, 0.1f,0.2f);
    
    glBegin(GL_QUADS);
      glVertex3f(0,0,0);
      glVertex3f(width,0,0);
      glVertex3f(width,0,depth);
      glVertex3f(0,0,depth);
    glEnd();
    
    glDisable(GL_BLEND);
  glPopMatrix();

}