//
//  FSWorld.cpp
//  FabScan
//
//  Created by Francis Engelmann on 7/3/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#include "FSWorld.h"
#include "FSCamera.h"
#include "FSLaser.h"
#include "FSModel.h"

#include <algorithm>
// --------------------------------------------------
#pragma mark -
#pragma mark Constructors and Destructors
// --------------------------------------------------

FSWorld::FSWorld()
{
  printf("FSWorld constructor start \n");
  camera = new FSWorldCamera();
  printf("FSWorld constructor end \n");
}

FSWorld::~FSWorld()
{
  delete camera;
}

// --------------------------------------------------
#pragma mark -
#pragma mark Getters and Setters
// --------------------------------------------------


void FSWorld::drawObject(FSObject* object)
{
  object->draw();
}

void FSWorld::init(){
  printf("FSWorld init start\n");

  //clean up from visionView
  glDisable( GL_TEXTURE_2D );

  //glClearColor(0.0f, 0.137f, 0.467f, 1.0f); //dark blue
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClearDepth(1.0f);
  glDepthFunc(GL_LESS);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_SHADING_LANGUAGE_VERSION_ARB);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  
  float lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};    //yellow diffuse : color where light hit directly the object's surface
  //float lightAmbient[] = {1.0f, 0.3f, 0.3f, 1.0f};    //red ambient : color applied everywhere
  float lightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};    //red ambient : color applied everywhere
  float lightPosition[]= {50, 100, 200, 0};
	
  //float matSpecular[]  = {0.5f,0.5f,0.5f,1.0f};
  //float matShininess[] = {15.0f};
	
  //glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
  //glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
	
  //Ambient light component
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  //Diffuse light component
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
  //Light position
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  glEnable(GL_LIGHT0);
  
  printf("FSWorld init end\n");
}

void FSWorld::draw(void){
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  /* drawing background gradient */
  
  glDisable(GL_DEPTH_TEST);
  
  float aspect;
  if (viewHeight!=0){
    aspect = (float)viewWidth / viewHeight;
  }

  glBegin(GL_QUADS);
  //red color
  
  //glColor3f(0.0f,0.8f,1.0f);
  //glColor3f(0.4f,0.4f,0.4f);
  glColor3f(1.0f,1.0f,1.0f);

  glVertex3f(-1.0f*aspect,-1.0f,-1.0f);
  glVertex3f(1.0f*aspect,-1.0f,-1.0f);
  //blue color
//  glColor3f(0.0f,0.24f,0.63f);
  //glColor3f(0.1f,0.1f,0.1f);
  glColor3f(1.0f,1.0f,1.0f);
  
  glVertex3f(1.0f*aspect, 1.0f,-1.0f);
  glVertex3f(-1.0f*aspect, 1.0f,-1.0f);
  glEnd();

  glEnable(GL_DEPTH_TEST);

  FSPoint target = model->getCenterOfVolume();
  //printf("%s target %f %f %f \n",__PRETTY_FUNCTION__,target.x,target.y,target.z);
  //target.x = 0.0;
  //target.y = 4.0;
  target.z += 7.5f;
  //camera->setTarget(target);
  camera->activate();  

  glColor3f(1.0f, 0.0f, 0.0f);
  //std::for_each(objects.begin(), objects.end(), drawObject);
  
  for(list<FSObject*>::iterator p = objects.begin(); p!=objects.end(); p++){
    if( (*p)->shouldDisplay() == 1 ){
      (*p)->draw();
    }
  }
  
  //drawPoint();
}

void FSWorld::setModel(FSModel * model)
{
  this->model = model;
}

void FSWorld::addObject(FSObject* object)
{
  objects.push_back(object);
}

// ---------------------------------------------
#pragma mark -
#pragma mark OpenGL Stuff
// ---------------------------------------------

void FSWorld::reshape(float width, float height) {
  float aspect = width / height;
  viewWidth = width;
  viewHeight = height;
  glViewport(0, 0, width, height);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (GLfloat)aspect, 0.1f, 100.0f);
    
  this->draw();
}

// ----------------------------------------
#pragma mark -
#pragma mark Camera modifiers
// ----------------------------------------

void FSWorld::zoom(FSFloat delta){
  //printf("FSWorld zoom \n");
  camera->setDistance(camera->getDistance()-delta);
}

void FSWorld::rotate(FSFloat delta){
  FSPoint p = camera->getUpVector(); 
  p.x = p.x+(float)sin(delta);
  p.y = p.y+(float)cos(delta);
  camera->setUpVector(p);
}

void FSWorld::rotateX(FSFloat delta){
  camera->setAngleX(camera->getAngleX()+delta);
}

void FSWorld::rotateY(FSFloat delta){
  camera->setAngleY(camera->getAngleY()+delta);
}

// ----------------------------------------
#pragma mark -
#pragma mark obsolete stuff
// ----------------------------------------

//obsolete replaced by box class
void FSWorld::drawGrid(){
  glPushMatrix();
  glTranslated(15, 0, 15);
  
    glLineWidth(1.0f);
    float n = 15.0f;
    glBegin(GL_LINES);
      glColor3f( 1.0f, 0.7f, 0.0f);
      for(int i=-n;i<=n;i++){
        glVertex3f(i, 0.0f, n);
        glVertex3f(i, 0.0f, -n);
        glVertex3f(n, 0.0f, i);
        glVertex3f(-n, 0.0f, i);
      }
    glEnd();
    
    glEnable(GL_BLEND); //Enable blending.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.
    glColor4f( 1.0f, 1.0f, 0.1f,0.2f);
    
    glBegin(GL_QUADS);
      glVertex3f(-n,0,-n);
      glVertex3f(-n,0,n);
      glVertex3f(n,0,n);
      glVertex3f(n,0,-n);
    glEnd();
    
    glDisable(GL_BLEND);
  glPopMatrix();
}

void FSWorld::drawFSPoint(FSPoint p)
{  
  glPushMatrix();
      glTranslated(p.x, p.y, p.z);
      glutSolidSphere(0.5f,10.0f, 10.0f);
  glPopMatrix();
}

void FSWorld::drawPoint()
{
  list<FSObject*>::iterator p = objects.begin();
  FSCamera* camera = (FSCamera*)(*p);
  p++;
  FSLaser* laser = (FSLaser*)(*p);

  FSPoint camPos = camera->getPosition();
  FSPoint laserPos = laser->getPosition();
  FSPoint laserFrame = laser->getLaserPointPosition();
  FSPoint camFrame = FSMakePoint(5,camPos.y,0);

  glColor3f(1.0f, 0, 0);
  drawFSPoint(camPos);
  glColor3f(0, 1, 0);
  drawFSPoint(laserPos);
  glColor3f(1, 1, 0);
  drawFSPoint(camFrame);
  glColor3f(1, 1, 1);
  drawFSPoint(laserFrame);
  
  glBegin(GL_LINES);
    glVertex3f(camPos.x, camPos.y, camPos.z);
    glVertex3f(camFrame.x, camFrame.y, camFrame.z);
    glVertex3f(laserFrame.x, laserFrame.y, laserFrame.z);
    glVertex3f(laserPos.x, laserPos.y, laserPos.z);
  glEnd();
  
  //Line between camera and the projected laser point on the object the camera sees 
  FSLine2 l1 = computeLineFromPoints(camPos, camFrame);
  //Line between the laser and the laser point if it would not have been projected on the object
  FSLine2 l2 = computeLineFromPoints(laserPos, laserFrame);
  
  FSPoint i = computeIntersectionOfLines(l1, l2);
  i.y = camPos.y; //put it on the same plane as the other points
  
  drawFSPoint(i);
}

//obsolete, replaced by chessbaord class
void FSWorld::drawChessboard(){    
  for(int x=-15;x<0;x++){
    for(int y=0;y<10;y++){
      if((x+y)%2){
          glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      }else{
          glColor4f(0.0f, 0.0f, 0.0f,1.0f);
      }
      glBegin(GL_QUADS);
        glVertex3f(x+0,y+0,0);
        glVertex3f(x+0,y+1,0);
        glVertex3f(x+1,y+1,0);
        glVertex3f(x+1,y+0,0);
      glEnd();
    }
  }
}


