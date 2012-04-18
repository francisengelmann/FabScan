//
//  FSGeometry.h
//  FabScan3D
//
//  Created by Francis Engelmann on 7/2/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_GEMOETRY_H
#define FS_GEMOETRY_H

typedef float FSFloat; //in cm
typedef unsigned char FSUChar; //in cm
typedef unsigned char FSBool;

// ----------------------------------------
#pragma mark -
#pragma mark FSColor
// ----------------------------------------

typedef struct _FSColor
{
  FSUChar red;
  FSUChar green;
  FSUChar blue;
} FSColor;

static FSColor FSMakeColor(FSUChar red, FSUChar green, FSUChar blue)
{
    FSColor c;
    c.red = red;
    c.green = green;
    c.blue = blue;
    return c;
}

// ----------------------------------------
#pragma mark -
#pragma mark FSPoint
// ----------------------------------------

typedef struct _FSPoint
{
  FSFloat x;
  FSFloat y;
  FSFloat z;
  FSColor color;
} FSPoint;

static FSPoint FSMakePoint(FSFloat x, FSFloat y, FSFloat z)
{
    FSPoint p;
    p.x = x;
    p.y = y;
    p.z = z;
    return p;
}

//typedef std::Vec<int, 4> Vec4i;
 
// ----------------------------------------
#pragma mark -
#pragma mark FSLine2
// ----------------------------------------

typedef struct _FSLine2
{
  FSFloat a;
  FSFloat b;
} FSLine2;

static FSLine2 FSMakeLine2(FSFloat a, FSFloat b)
{
    FSLine2 l;
    l.a = a;
    l.b = b;
    return l;
}

// ----------------------------------------
#pragma mark -
#pragma mark FSSize
// ----------------------------------------

typedef struct _FSSize
{
  FSFloat width;
  FSFloat height;
  FSFloat depth;
} FSSize;

static FSSize FSMakeSize(FSFloat width, FSFloat height, FSFloat depth)
{
    FSSize s;
    s.width = width;
    s.height = height;
    s.depth = depth;
    return s;
}

// ----------------------------------------
#pragma mark -
#pragma mark Handy functions
// ----------------------------------------


//points must have same height
static FSLine2 computeLineFromPoints(FSPoint p1, FSPoint p2)
{
  FSLine2 l;
  l.a = (p2.z-p1.z)/(p2.x-p1.x);
  l.b = p1.z-l.a*p1.x;
  return l;
}

//lines must be on same plane
static FSPoint computeIntersectionOfLines(FSLine2 l1, FSLine2 l2)
{
  FSPoint i; //intersection of the two coplanar lines
  i.x = (l2.b-l1.b)/(l1.a-l2.a);
  i.z = l2.a*i.x+l2.b;
  return i;
}

typedef enum {
  FS_DIRECTION_CCW,
  FS_DIRECTION_CW
} FSDirection;

#endif