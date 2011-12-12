/*
 *  FSWorldCamera.h
 *
 *  Created by Francis Engelmann on 3/19/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
 *
 */

#ifndef FS_WORLD_CAMERA_H
#define FS_WORLD_CAMERA_H

#include "FSHeaders.h"

class FSWorldCamera {
    private:
        FSPoint position; //position of camera
        FSPoint target;   //position of target
        FSPoint upVector; //up vector of camera
        FSFloat distance; //distance between camera and target
        FSFloat angleX;   //angle of camera when rotating around target
        FSFloat angleY;   //angle of camera when rotating around target
    public:
        /* Constrcutors and Destructors */
        FSWorldCamera();
        ~FSWorldCamera();
        
        /* Getters and Setters */
        void setPosition(FSPoint position);
        FSPoint getPosition(void);
        
        void setTarget(FSPoint target);
        FSPoint getTarget(void);
        
        void setUpVector(FSPoint upVector);
        FSPoint getUpVector(void);
        
        void setDistance(FSFloat distance);
        FSFloat getDistance(void);
        
        void setAngleX(FSFloat angle);
        FSFloat getAngleX(void);
        
        void setAngleY(FSFloat angle);
        FSFloat getAngleY(void);        
        
        /* Methods */
        void activate(void);
};

#endif