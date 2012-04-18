//
//  FSSurfaceMesh.h
//  FabScan
//
//  Created by Francis Engelmann on 9/12/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_SURFACE_MESH_H
#define FS_SURFACE_MESH_H

#include "FSHeaders.h"
#include "FSObject.h"

using namespace std;

class FSModel;

class FSSurfaceMesh : public FSObject {
  
  private:
    FSModel* model;
    vector<FSPoint> vertexVector;               // contains the points/vertices from a .off file [v_1][v_2][v_3]...
    vector<vector <unsigned int> > faceVector;  // contains the indices to define faces [[v_a][v_b]...] [[v_c][v_d]...]...
  
  public:
    FSSurfaceMesh();
    ~FSSurfaceMesh();
    
    void setModel(FSModel* model);


    unsigned int openFromOFFFile(const char* offFilePath);
    unsigned int convertPolygons2Triangles(void);
    unsigned int saveToSTLFile(const char* stlFilePath);
    unsigned int saveToPLYFile(const char* plyFilePath);
    unsigned int saveToSCADFile(const char* scadFilePath);

    void draw();
    void clearAll();
};

#endif