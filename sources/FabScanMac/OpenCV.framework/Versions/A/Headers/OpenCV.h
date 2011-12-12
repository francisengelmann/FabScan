// This file is the meta-header for OpenCV that should include
// everything from the project. However, currently the headers
// for ml and cvaux are broken and do not compile

#ifndef FRAMEWORK_OPENCV_H
#define FRAMEWORK_OPENCV_H

// allow new API only by default
#define CV_NO_BACKWARD_COMPATIBILITY

//#include <OpenCV/cxcore.h>  // included by cv.h, ml.h and highgui.h
//#include <OpenCV/cv.h>        // included by cvaux.h
#include <OpenCV/cvaux.h>
#include <OpenCV/ml.h>
#include <OpenCV/highgui.h>

#endif // FRAMEWORK_OPENCV_H
