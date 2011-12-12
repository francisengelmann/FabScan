//
//  FSChessboardCalibration.h
//  FabScan3D
//
//  Created by Francis Engelmann on 7/8/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_CHESSBOARD_CALIBRATION
#define FS_CHESSBOARD_CALIBRATION

#include "FSHeaders.h"

class FSChessboardCalibration {
  private:
    int n_boards; //the number of boards used for calibration
    int board_dt; // Wait 20 frames per chessboard view
    int board_w;
    int board_h;
    int board_n;
    CvSize board_sz;
    CvSize image_sz;
    
    CvMat* image_points;
    CvMat* object_points;
    CvMat* point_counts;
    CvMat* intrinsic_matrix;
    CvMat* distortion_coeffs;
    
    CvPoint2D32f* corners;
    int corner_count;
    int successes;
    int step;
    int frame;
    
    CvMat* object_points2;
    CvMat* image_points2;
    CvMat* point_counts2;
    
    const char* resPath;
    
  public:
    FSChessboardCalibration(const char* resPath);
    ~FSChessboardCalibration(void);

    CvMat* getInstrinsicMatrix(void);
    CvMat* getDistortionCoeffs(void);

    int needMoreBoardsForCalibration(void);
    
    /**
     * mark the corners in the specified frame and collect the info for later calibration
     * @return the index of correctly recognized chessboard
     */
    int findBoardInFrame(IplImage* frame);
    
    /**
     * calibrates the camera with premade pictures
     * @return unknown
     */
    int autoCalibrate();
    
    //no return needed here, as this is used to find position of chessboard relative to camera
    //obsolete
    IplImage* findBoardInFrame2(IplImage* image, CvMat* intrinsic_matrix, CvMat* distortion_coeffs, CvMat* rotation_vector, CvMat* translation_vector);
      
    FSPoint calibrateCamera();
    FSPoint findExtrinsics(IplImage* image);

};

#endif