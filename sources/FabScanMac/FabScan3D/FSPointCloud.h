//
//  FSPointCloud.h
//  FabScan3D
//
//  Created by Francis Engelmann on 7/18/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_POINT_CLOUD_H
#define FS_POINT_CLOUD_H

#include "FSHeaders.h"
#include "FSObject.h"
#include "FSTurnTable.h"

using namespace std;

class FSModel;

class FSPointCloud : public FSObject {
  private:
    FSModel* model;
    
    list<FSPoint> points;
    FSTurnTable* turntable;
    
  public:
    FSPointCloud();
    ~FSPointCloud();

    void setTurnTable(FSTurnTable* turntable);
    FSTurnTable* getTurnTable(void);

    void setModel(FSModel* model);

    unsigned char saveToPCDFile(const char* fileName);
    unsigned char saveToPTSFile(const char* fileName);
    unsigned char saveToPLYFile(const char* fileName);
    unsigned char saveToPLYMeshFile(const char* fileName);
    
    unsigned char openFromPCDFile(const char* filename);

    void addPoint(FSPoint point);
    void draw();
    void clearAll();
};

#endif