//
//  FSHeaders.h
//  FabScan3D
//
//  Created by Francis Engelmann on 7/3/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

// ---------------------------------------------
#pragma mark -
#pragma mark C++ Headers
// ---------------------------------------------

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>

// ---------------------------------------------
#pragma mark -
#pragma mark OpenGL Headers
// ---------------------------------------------

#ifdef __APPLE__
  #include <GLUT/glut.h>
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <OpenGL/glext.h>
#else
  #include <GL/glut.h>
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/glext.h>
#endif

// ---------------------------------------------
#pragma mark -
#pragma mark OpenCV Headers
// ---------------------------------------------

#include <OpenCV/OpenCV.h>

// ---------------------------------------------
#pragma mark -
#pragma mark FabScan Headers
// ---------------------------------------------

#include "FSDefines.h"
#include "FSGeometry.h"