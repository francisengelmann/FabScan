//
//  FSPointCloud.cpp
//  FabScan3D
//
//  Created by Francis Engelmann on 7/18/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#include "FSPointCloud.h"
#include "FSModel.h"

FSPointCloud::FSPointCloud()
{
  position.x = 0.0f;
  position.y = 0.0f;
  position.z = 0.0f;
  
  /*minPoint = FSMakePoint(REALLY_BIG_NUMBER, REALLY_BIG_NUMBER, REALLY_BIG_NUMBER);
  maxPoint = FSMakePoint(REALLY_SMALL_NUMBER, REALLY_SMALL_NUMBER, REALLY_SMALL_NUMBER);*/
}

FSPointCloud::~FSPointCloud()
{
}

void FSPointCloud::setTurnTable(FSTurnTable* turntable)
{
  this->turntable = turntable;
}

FSTurnTable* FSPointCloud::getTurnTable(void)
{
  return turntable;
}

void FSPointCloud::setModel(FSModel* model)
{
  this->model = model;
}

void FSPointCloud::addPoint(FSPoint point)
{
  model->updateMinMax(point);
  points.push_back(point);
}

unsigned char FSPointCloud::saveToPCDFile(const char* filename)
{
  ofstream pcdFile;

  string pcdFilename;
  
  pcdFilename.assign(filename);
  
  //pcdFilename.append(".pcd");
  
  pcdFile.open (pcdFilename.c_str());
  
  //PCD File Format check thessis for more info
  
  pcdFile << "# .PCD v.7 - Point Cloud Data file format\n";
  pcdFile << "VERSION .7\n";
  pcdFile << "FIELDS x y z rgb\n";
  pcdFile << "SIZE 4 4 4 4\n"; //4 because FSFloat is currently defined as float
  pcdFile << "TYPE F F F F\n";
  pcdFile << "COUNT 1 1 1 1\n";
  pcdFile << "WIDTH  " << points.size() << "\n"; //width can have two meanings, here it is the number of points
  pcdFile << "HEIGHT 1\n"; //height has two meanings, 1 means unorgnaized point cloud
  pcdFile << "VIEWPOINT 0 0 0 1 0 0 0\n"; //not used
  pcdFile << "POINTS " << points.size() << "\n";
  pcdFile << "DATA ascii\n";
  
  /*for (size_t i = 0; i < cloud->points.size(); ++i){
    uint32_t rgb = *reinterpret_cast<int*>(&cloud->points[i].rgb);
    uint8_t r = (rgb >> 16) & 0x0000ff;
    uint8_t g = (rgb >> 8)  & 0x0000ff;
    uint8_t b = (rgb)       & 0x0000ff;
    std::cout << " " << cloud->points[i].x
              << " " << cloud->points[i].y
              << " " << cloud->points[i].z
              << " " << (int)r
              << " " << (int)g
              << " " << (int)b
              << " " << cloud->points[i].imX
              << " " << cloud->points[i].imY << std::endl;
  }*/

  for (list<FSPoint>::const_iterator iterator = points.begin(); iterator != points.end(); iterator++){
    FSPoint p =  *iterator;
    uint32_t rgb = (p.color.red << 16) + (p.color.green << 8) + p.color.blue;
    
    //FSUChar r = (rgb >> 16) & 0x0000ff;
    //FSUChar g = (rgb >> 8)  & 0x0000ff;
    //FSUChar b = (rgb >> 0)  & 0x0000ff;
    
    //printf("COLOR %u %u %u \n",r,g,b);
    
    pcdFile << p.x << " " << p.y << " " << p.z <<  " " << rgb << endl;
  }
  pcdFile.close();
  return 0;
}

unsigned char FSPointCloud::saveToPLYFile(const char* filename)
{
  ofstream plyFile;
  string plyFilename;
  
  plyFilename.assign(filename);
  //plyFilename.append(".pcd");
  
  plyFile.open (plyFilename.c_str());
  
  //PCD File Format check thessis for more info
  
  plyFile << "ply\n";
  plyFile << "format ascii 1.0\n";
  plyFile << "element vertex " << points.size() << "\n";;
  plyFile << "property float x\n";
  plyFile << "property float y\n";
  plyFile << "property float z\n";
  plyFile << "property uchar diffuse_red\n";
  plyFile << "property uchar diffuse_green\n";
  plyFile << "property uchar diffuse_blue\n";
  plyFile << "element face 0\n";
  plyFile << "property list uchar int vertex_indices\n";
  plyFile << "end_header\n";

  for (list<FSPoint>::const_iterator iterator = points.begin(); iterator != points.end(); iterator++){
    FSPoint p =  *iterator;
    plyFile << p.x << " " << p.y << " " << p.z <<  " "
    << (int)p.color.red <<  " " << (int)p.color.green <<  " " << (int)p.color.blue << endl;
  }
  plyFile.close();
  return 0;
}


unsigned char FSPointCloud::saveToPTSFile(const char* filename)
{
  ofstream ptsFile;

  string ptsFilename;
  
  ptsFilename.assign(filename);
  
  //ptsFilename.append(".pts");
  
  ptsFile.open (ptsFilename.c_str());
  
  for (list<FSPoint>::const_iterator iterator = points.begin(); iterator != points.end(); iterator++){
    FSPoint p =  *iterator;
    ptsFile << p.x << " " << p.y << " " << p.z << endl;
  }
  ptsFile.close();
  return 0;
}

unsigned char FSPointCloud::openFromPCDFile(const char* ptsFilePath)
{
  /* old stuff
  string line;
  ifstream myfile ("pointcloud.pts");
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
      getline (myfile,line);
      //size_t pos = line.find(" ");    // position of "live" in str
      //str3 = str.substr (pos);
      cout << line << endl;
    }
    myfile.close();
  }
  else cout << "Unable to open file"; */
  //this->ptsFilePath = ptsFilePath;
  //asprintf(&(this->ptsFilePath), ptsFilePath);
  //cout << this->ptsFilePath << endl;
  
  ifstream ptsFile; //the file object (read only)
  ptsFile.open(ptsFilePath,ios::in); //open file for input
  
  //pointList.clear(); //make sure the list is empty before reading new file
  
  /* jump over the header as it is always the same anyway, this can be improved when using the pcl library */
  string line;
  for(int i=0; i < 11; i++){
    if(ptsFile.eof()){ return 1; }
    getline(ptsFile,line);
  }
    
  FSPoint p;//the point we are currently reading from the line
  while(ptsFile.peek()!=EOF){ //keep reading until end of file
    
    // the >>-operator reads until space or return char and converts to correct type, here
    uint32_t rgb;

    ptsFile >> p.x; if(ptsFile.eof()){ return 1; } //read x compononent or return error if prematurely ended
    ptsFile >> p.y; if(ptsFile.eof()){ return 1; } //read y ...
    ptsFile >> p.z; if(ptsFile.eof()){ return 1; } //read z ...
    ptsFile >> rgb; if(ptsFile.eof()){ return 1; } //read rgb ...

    p.color.red   = (rgb >> 16) & 0x0000ff;
    p.color.green = (rgb >> 8)  & 0x0000ff;
    p.color.blue  = (rgb >> 0)  & 0x0000ff;

    ptsFile.ignore(256, '\n');
    //at this point, 3 points where correctly read
    this->addPoint(p);
  }
  ptsFile.close();  //close file
  return 0;         //no errors
}

void FSPointCloud::draw()
{
  FSPoint rot = turntable->getRotation();
  FSPoint pos = turntable->getPosition();
  glPushMatrix();
  glTranslated(pos.x, pos.y, pos.z);
  glRotated(rot.y , 0, 1, 0);
  glBegin(GL_POINTS);
  for (list<FSPoint>::const_iterator iterator = points.begin(); iterator != points.end(); iterator++){
    FSPoint p =  *iterator;
    //printf("%s %f %f %f \n",__PRETTY_FUNCTION__,p.x, p.y, p.z);
    glColor3ub(p.color.red, p.color.green, p.color.blue);
    glVertex3f(p.x, p.y, p.z);
  } 
  glEnd();
  glPopMatrix();
}

void FSPointCloud::clearAll()
{
  points.clear();
}