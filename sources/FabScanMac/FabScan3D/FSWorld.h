//
//  FSWorld.h
//  FabScan
//
//  Created by Francis Engelmann on 7/3/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_WORLD_H
#define FS_WORLD_H

#include "FSHeaders.h"

#include "FSWorldCamera.h"
#include "FSObject.h"
#include "FSFrame.h"
#include "FSSerial.h"

using namespace std;

class FSModel;

class FSWorld {
  private:
    FSModel* model;
    FSWorldCamera* camera;
    list<FSObject*> objects;
    void drawObject(FSObject* object);
    void drawFSPoint(FSPoint p);
    
    int viewWidth;  //width of the openglview in pixels
    int viewHeight; //height of the openglview in pixels
    
  public:
    FSWorld();
    ~FSWorld();
    
    void addObject(FSObject* object);
    
    void setModel(FSModel * model);
    
    /* OpenGL Stuff */
    void init();
    void draw(void);
    void reshape(float width, float height);
    
    /* Zooming and Rotating */
    void zoom(FSFloat delta);  
    void rotate(FSFloat delta); 
    void rotateX(FSFloat delta);
    void rotateY(FSFloat delta);
    
    //obsolete, replaced by box
    void drawGrid();
    //obsolete, replaced by class
    void drawChessboard();
    
    void drawPoint();
};

#endif