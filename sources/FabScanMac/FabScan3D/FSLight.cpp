//
//  FSLight.cpp
//  FabScan
//
//  Created by Francis Engelmann on 9/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "FSLight.h"

FSLight::FSLight(FSSerial2* serial)
{
  this->serial = serial;
}

FSLight::~FSLight()
{
  
}

void FSLight::turnOn(unsigned char intensity)
{
  serial->writeByte(MC_TURN_LIGHT_ON);
  serial->writeByte(intensity);
}

void FSLight::turnOn()
{
  this->turnOn(255);
}

void FSLight::turnOff()
{
  serial->writeByte(MC_TURN_LIGHT_OFF);
}