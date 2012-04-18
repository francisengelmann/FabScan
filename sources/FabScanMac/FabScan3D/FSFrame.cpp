//
//  FSFrame.cpp
//  FabScan3D
//
//  Created by Francis Engelmann on 7/3/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#include "FSFrame.h"

// --------------------------------------------------
#pragma mark -
#pragma mark Constrcutors and Destructors
// --------------------------------------------------



FSFrame::FSFrame()
{
  frame=NULL;
  size = FSMakeSize(14.0f, 14.0f, 0.0f);
  position = FSMakePoint(0.0f, 0.0f, 0.0f);
}

FSFrame::FSFrame(FSPoint p)
{
  frame=NULL;
  //size will be reset
  size = FSMakeSize(FRAME_WIDTH, FRAME_WIDTH, 0.0f);
  position = p;
}


FSFrame::~FSFrame()
{
}

// --------------------------------------------------
#pragma mark -
#pragma mark Getters and Setters
// --------------------------------------------------

void FSFrame::setFrame(IplImage* frame)
{
  if(frame == NULL){
    this->frame = NULL;
    return;
  }
  //cout << frame->width << " " << frame->height << " " << endl;
  size = FSMakeSize((FSFloat)frame->width/(FSFloat)frame->width*size.width, (FSFloat)frame->height/(FSFloat)frame->width*size.width, 0.0f);
  //cout << __PRETTY_FUNCTION__ << size.width << " " << size.height << " " << endl;
  //position.x = -(size.width)/2.0;
  cvReleaseImage(&(this->frame));//remove old frame befor setting new one
  this->frame = frame;
}

IplImage* FSFrame::getFrame(void)
{
  return frame;
}

FSSize FSFrame::getSize(void)
{
  return size;
}

void FSFrame::setSize(FSSize size)
{
  this->size = size;
}



// --------------------------------------------------
#pragma mark -
#pragma mark draw
// --------------------------------------------------

void FSFrame::draw(void)
{
  //cout << __PRETTY_FUNCTION__ << " camera draw "<< endl;
  if(frame==NULL) return; //when there is no frame set, don't display anything
  glPushMatrix();
  glTranslated(position.x, position.y, position.z);
  glEnable( GL_TEXTURE_2D );
    glBindTexture(GL_TEXTURE_2D, 13);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame->width, frame->height, 0, GL_BGR , GL_UNSIGNED_BYTE, frame->imageData);

    glBegin (GL_QUADS);
      glTexCoord2f (0.0f, 1.0f);
      glVertex3f (0.0f, 0.0f, 0.0f);
      
      glTexCoord2f (1.0f, 1.0f);
      glVertex3f (size.width, 0.0f, 0.0f);
      
      glTexCoord2f (1.0f, 0.0f);
      glVertex3f (size.width, size.height, 0.0f);
      
      glTexCoord2f (0.0f, 0.0f);
      glVertex3f (0.0f, size.height, 0.0f);
    glEnd ();
  glDisable( GL_TEXTURE_2D );
  glPopMatrix();
}