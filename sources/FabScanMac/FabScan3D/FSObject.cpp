//
//  FSObject.cpp
//  FabScan
//
//  Created by Francis Engelmann on 7/2/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#include "FSObject.h"

FSObject::FSObject(){
  display = 1;
}

FSObject::~FSObject(){

}

void FSObject::setPosition(FSPoint position)
{
  this->position = position;
}

FSPoint FSObject::getPosition()
{
  return position;
}

void FSObject::setRotation(FSPoint rotation)
{
  this->rotation = rotation;
}

FSPoint FSObject::getRotation()
{
  return rotation;
}

unsigned char FSObject::shouldDisplay(void)
{
  return display;
}

void FSObject::setShouldDisplay(int d)
{
  display = d;
}