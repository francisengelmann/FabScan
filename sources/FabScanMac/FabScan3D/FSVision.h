//
//  FSVision.h
//  FabScan3D
//
//  Created by Francis Engelmann on 7/4/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_VISION_H
#define FS_VISION_H

#include "FSHeaders.h"
#include "FSLaser.h"
#include "FSCamera.h"
#include "FSTurnTable.h"
#include "FSPointCloud.h"

class FSVision {
  private:
    
  public:
  
    static FSPoint convertCvPointToFSPoint(CvPoint cvPoint);
    static CvPoint convertFSPointToCvPoint(FSPoint fsPoint);
  
    static IplImage* subLaser(IplImage *frame, IplImage *laserFrame, FSFloat treshold);
    static void addGreenLineTo(IplImage *pframe);
    static void drawLaserLineFromLaserToImageSeenFromCamera(FSLaser* laser, IplImage *image, FSCamera* camera);
    
    static void putPointsFromFrameToCloud(IplImage* laserFrame, IplImage* noLaserFrame, FSPointCloud* pointCloud, FSLaser* laser, FSCamera* camera);
    static void putPointsFromFrameToCloud(IplImage* laserFrame, IplImage* noLaserFrame, FSPointCloud* pointCloud, FSLaser* laser, FSCamera* camera, FSTurnTable* turntable, int dpiVertical, FSFloat lowerLimit, FSFloat treshold);

};

#endif