//
//  FSCamera.h
//  FabScan3D
//
//  Created by Francis Engelmann on 7/2/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_CAMERA_H
#define FS_CAMERA_H

#include "FSHeaders.h"
#include "FSObject.h"
#include "FSChessboardCalibration.h"

class FSCamera : public FSObject {
  private:
    CvCapture* camera;
    CvMat *intrinsic; 
    CvMat *distortion;
    const char* resPath;

  public:
    /* resPath : path to distortion/intrinisics params */
    FSCamera(const char* resPath);
    ~FSCamera();
  
    void setIntrinsic(CvMat* intrinsic);
    CvMat* getIntrinsic(void);
    void setDistortion(CvMat* distortion);
    CvMat* getDistortion(void);
    
    IplImage* fetchFrame(void);
    IplImage* fetchQuickFrame(void);
    IplImage* fetchUndistortedFrame(void);
    
    IplImage* setPositionUsingChessboardCalibration(FSChessboardCalibration* cb);
    
    void calibrate(void);
    
    void draw(void);
};

#endif