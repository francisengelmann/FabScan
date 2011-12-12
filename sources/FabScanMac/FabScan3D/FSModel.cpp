//
//  FSModel.cpp
//  FabScan
//
//  Created by Francis Engelmann on 9/12/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#include "FSModel.h"

FSModel::FSModel(FSPointCloud* pointCloud, FSSurfaceMesh* surfaceMesh)
{
  minPoint = FSMakePoint(REALLY_BIG_NUMBER, REALLY_BIG_NUMBER, REALLY_BIG_NUMBER);
  maxPoint = FSMakePoint(REALLY_SMALL_NUMBER, REALLY_SMALL_NUMBER, REALLY_SMALL_NUMBER);
  this->pointCloud = pointCloud;
  this->surfaceMesh = surfaceMesh;
  
  pointCloud->setModel(this);
  surfaceMesh->setModel(this);
  
  surfaceMeshGenerated = NO;
  surfaceMeshLoaded = NO;
}


FSModel::~FSModel()
{
  return;
}

void FSModel::updateMinMax(FSPoint p)
{
  if(p.x < minPoint.x) minPoint.x=p.x;
  if(p.y < minPoint.y) minPoint.y=p.y;
  if(p.z < minPoint.z) minPoint.z=p.z;

  if(p.x > maxPoint.x) maxPoint.x=p.x;
  if(p.y > maxPoint.y) maxPoint.y=p.y;
  if(p.z > maxPoint.z) maxPoint.z=p.z;
}

FSPoint FSModel::getMinPoint(void)
{
  return minPoint;
}

FSPoint FSModel::getMaxPoint(void)
{
  return maxPoint;
}

FSPoint FSModel::getCenterOfVolume(void)
{
  FSPoint center;
  center.x = (minPoint.x+maxPoint.x)/2;
  center.y = (minPoint.y+maxPoint.y)/2; 
  center.z = (minPoint.z+maxPoint.z)/2;
  return center;
}


void FSModel::setPointCloud(FSPointCloud* pointCloud)
{
  this->pointCloud = pointCloud;
}

FSPointCloud* FSModel::getPointCloud(void)
{
  return pointCloud;
}
    
void FSModel::setSurfaceMesh(FSSurfaceMesh* surfaceMesh)
{
  this->surfaceMesh = surfaceMesh;
}

FSSurfaceMesh* FSModel::getSurfaceMesh(void)
{
  return surfaceMesh;
}

FSBool FSModel::getSurfaceMeshGenerated(void)
{
  return surfaceMeshGenerated;
}

FSBool FSModel::getSurfaceMeshLoaded(void)
{
  return surfaceMeshLoaded;
}

FSBool FSModel::getPolygons2TrianglesTransformed(void)
{
  return polygons2TrianglesTransformed;
}

void FSModel::setSurfaceMeshGenerated(FSBool b)
{
  this->surfaceMeshGenerated = b;
}

void FSModel::setSurfaceMeshLoaded(FSBool b)
{
  this->surfaceMeshLoaded = b;
}

void FSModel::setPolygons2TrianglesTransformed(FSBool b)
{
  this->polygons2TrianglesTransformed = b;
}

unsigned int FSModel::convertPTS2OFF(const char* resPath){
  //const char* cPath = "/Users/francis/Programming/FabScan/Libraries/powercrust/";
  //string filePath(this->ptsFilePath);
  //system("cd /Users/francis/Library/Developer/Xcode/DerivedData/FabScan-dadiuntwjnfqowgshevimymqbsnj/Build/Products/Debug");
  //system("pwd");
  cout << "Current path: " << resPath << endl; 
  
  char* ptsFilePath;
  asprintf(&ptsFilePath,"%s/pc.pts",resPath);
  pointCloud->saveToPTSFile(ptsFilePath);
  
  //string filePath("tmp.pts");
  //size_t found = filePath.find_last_of("/");
  //string fileName = filePath.substr(found+1);
  //const char* cFileName = fileName.c_str();
  //const char* intputPath = "/Users/francis/Programming/FabScan/Libraries/powercrust/cube3.pts";
  //execl(path,"-i",ptsFilePath,"-m","10000",(char *)0);
  //path is given 2 times a argv0 is process name
  char* command; 
  //asprintf(&command,"%s -i %s -m 10000",path,this->ptsFilePath);
  asprintf(&command,"cd %s; ./powercrust -i %s -R 1.5 -B -m 10000",resPath,"pc.pts");
  //"/Users/francis/Programming/FabScan/Libraries/powercrust/powercrust";
  //printf("selected pts file: %s",this->ptsFilePath);
  //pid_t pID = fork();
  int sysRet = system(command);
  cout << command << " system: " << sysRet << endl;
  if(sysRet==0){
    surfaceMeshGenerated = YES;
  }
  //const char* offFilePath = "/Users/francis/Programming/FabScan/Libraries/powercrust/pc.off";
  /*
  if(pID==0){//child process
    cout << "i am child process" << endl;
    sleep(4);
    execl("bash",path,"-i",this->ptsFilePath,"-o",offFilePath,"-m","10000",(char *)0);
    cout << "finished child execution" << endl;
    exit(EXIT_SUCCESS);

  }else{//parent process
    int asd;
    wait(&asd);
    cout << "finished aiting for client" << endl;
  }*/
  
  asprintf(&command,"cd %s; ./orient -i pc.off -o final.off",resPath);
  sysRet = system(command);
  cout << command << " system: " << sysRet << endl;
  
  char* offFilePath;
  asprintf(&offFilePath,"%s/final.off",resPath);
  surfaceMesh->openFromOFFFile(offFilePath);
  if(sysRet==0){
    surfaceMeshLoaded = YES;
  }
  return 0;
}

void FSModel::close(void)
{
  pointCloud->clearAll();
  surfaceMesh->clearAll();
  surfaceMeshGenerated = NO;
  surfaceMeshLoaded = NO;
  polygons2TrianglesTransformed = NO;
}

