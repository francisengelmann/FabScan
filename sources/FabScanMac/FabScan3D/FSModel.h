//
//  FSModel.h
//  FabScan
//
//  Created by Francis Engelmann on 9/12/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_MODEL_H
#define FS_MODEL_H

#include "FSHeaders.h"
#include "FSPointCloud.h"
#include "FSSurfaceMesh.h"

using namespace std;

class FSModel {

  private:
    FSPointCloud* pointCloud;
    FSSurfaceMesh* surfaceMesh;
    
    FSPoint minPoint;
    FSPoint maxPoint;
 
    FSBool surfaceMeshGenerated;
    FSBool surfaceMeshLoaded;
    FSBool polygons2TrianglesTransformed;
      
  public:
    /* Constructor & Desctructor */
    FSModel(FSPointCloud* pointCloud, FSSurfaceMesh* surfaceMesh);
    ~FSModel();
    
    /* Getters & Setters */
    void setPointCloud(FSPointCloud* pointCloud);
    FSPointCloud* getPointCloud(void);
    
    void setSurfaceMesh(FSSurfaceMesh* surfaceMesh);
    FSSurfaceMesh* getSurfaceMesh(void);
    
    FSBool getSurfaceMeshGenerated(void);
    FSBool getSurfaceMeshLoaded(void);
    FSBool getPolygons2TrianglesTransformed(void);

    void setSurfaceMeshGenerated(FSBool);
    void setSurfaceMeshLoaded(FSBool);
    void setPolygons2TrianglesTransformed(FSBool);
    
    void updateMinMax(FSPoint point);
    FSPoint getMinPoint(void);
    FSPoint getMaxPoint(void);
    
    FSPoint getCenterOfVolume(void);
    
    /**
     * calls the command to convert the .pts to .off
     * and reads resulting .off file
     * @return to be defined
     */
    unsigned int convertPTS2OFF(const char* resPath);
      
    /**
     * converts each polygon of the loaded model into triangles  
     * (needed for .stl)
     */
    //unsigned int convertPolygons2Triangles(void);
       
    /**
     * export loaded and triangulated model to SDL
     */
    //unsigned int exportSTL(const char* stlFilePath);

    void close(void);
};

#endif