//
//  FSSurfaceMesh.cpp
//  FabScan
//
//  Created by Francis Engelmann on 9/12/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#include "FSSurfaceMesh.h"
#include "FSModel.h"

FSSurfaceMesh::FSSurfaceMesh()
{
}

FSSurfaceMesh::~FSSurfaceMesh()
{}

void FSSurfaceMesh::setModel(FSModel* model)
{
  this->model = model;
}


unsigned int FSSurfaceMesh::openFromOFFFile(const char* offFilePath){
  //cout << offFilePath << endl;
  ifstream offFile; //the file object (read only)
  offFile.open(offFilePath,ios::in); //open file for input
  
  /* read first line and checks wether it is correct or not according to .off format */
  string fileFormat;
  offFile >> fileFormat; //first line/word of a .off should be OFF
  printf("%s filePath %s \n",__PRETTY_FUNCTION__,offFilePath);
  if(fileFormat.compare("OFF") != 0){ //if first line is not OFF
    printf("provided file is not a valid .off file \n");
    return 1;
  }
  
  /* clear old loaded model before opening new one */
  vertexVector.clear();
  faceVector.clear();
  
  /* read 2. line, containg number of vertices, faces and edges */
  unsigned int numberOfVertices;  //needed to determine size of vector
  unsigned int numberOfFaces;     //needed to determine size of vector
  unsigned int numberOfEdges; //not needed
  
  offFile >> numberOfVertices;
  offFile >> numberOfFaces;
  offFile >> numberOfEdges;
  
  /* reserver space for vertexVector and faceVector according to numberOfVertices and numberOfEdges */
  vertexVector.reserve(numberOfVertices);
  faceVector.reserve(numberOfFaces);
  
  /* read vertices=tripple of points from .off file and store them in vertexVector */
  for(int i=0;i<numberOfVertices;i++){
    FSFloat x,y,z;
    while(offFile.peek()=='#') offFile.ignore(256,'\n');
    offFile >> x;
    while(offFile.peek()=='#') offFile.ignore(256,'\n');
    offFile >> y;
    while(offFile.peek()=='#') offFile.ignore(256,'\n');
    offFile >> z;
    offFile.ignore(256,'\n'); //ignore rest of line
    FSPoint point = FSMakePoint(x,y,z); //TODO: delete points in decronstructor
    vertexVector.push_back(point);
  }
  
  /* read faces from .off file and store them in faceVector */
  for(int i=0;i<numberOfFaces;i++){
    unsigned int n; //number of vertices on this face
    offFile >> n;
    vector<unsigned int> indiceVector;
    indiceVector.reserve(n);
    for(int j=0;j<n;j++){
      unsigned int z; //indice of vertex
      while(offFile.peek()=='#') offFile.ignore(256,'\n');
      offFile >> z;
      indiceVector.push_back(z); //add this vertex to vector of vertices per face
    }
    offFile.ignore(256,'\n'); //ignore rest of line
    faceVector.push_back(indiceVector);
  }
  //if(fileFormat !=)
  //if(fileFormat != "OFF"){ cout << "error wrong file format" << endl; return 1; }
  offFile.close();
  return 0;
}

void FSSurfaceMesh::draw(){

  /*if(model->getSurfaceMeshGenerated() == NO){
    return;
  }*/
  /* translate model */

  glPushMatrix();
  glTranslated(0, 0, 7.3);
  FSPoint maxPoint = model->getMaxPoint();
  FSPoint minPoint = model->getMinPoint();
  FSFloat x,y,z;
  x = (maxPoint.x - minPoint.x)/2;
  x += minPoint.x;
  y = (maxPoint.y - minPoint.y)/2;
  y += minPoint.y;
  z = (maxPoint.z - minPoint.z)/2;
  z += minPoint.y;
  //glTranslatef(-x,-y,-z);

  for(int i=0;i<faceVector.size();i++){
    glColor3f(0.5f, 1.0f, 0.0f);
    
    FSPoint p1 = vertexVector[ faceVector[i][0] ];
    FSPoint p2 = vertexVector[ faceVector[i][1] ];
    FSPoint p3 = vertexVector[ faceVector[i][2] ];
    
    //generate the 3 vectors from the polygon
    FSPoint v1,v2,n;
    v1.x=p1.x-p2.x;
    v1.y=p1.y-p2.y;
    v1.z=p1.z-p2.z;

    v2.x=p2.x-p3.x;
    v2.y=p2.y-p3.y;
    v2.z=p2.z-p3.z;
        
    n.x = v1.y*v2.z - v1.z*v2.y;
    n.y = v1.z*v2.x - v1.x*v2.z;
    n.z = v1.x*v2.y - v1.y*v2.x;
    
    FSFloat d = sqrtf(n.x*n.x+n.y*n.y+n.z*n.z);
    
    glEnable(GL_LIGHTING);

    n.x /= d;
    n.y /= d;
    n.z /= d;
    
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(n.x, n.y, n.z);

    for(int j=0;j<faceVector[i].size();j++){
      unsigned int indice = faceVector[i][j];
      FSPoint p = vertexVector[indice];
      
      glVertex3f(p.x,p.y,p.z);
      
    }
    
    glEnd();
    
    glDisable(GL_LIGHTING);
    
    /*glColor3f(0.0f, 0.5f, 0.0f);
    glLineWidth(1);
    glBegin(GL_LINE_STRIP);
    for(int j=0;j<faceVector[i].size();j++){
      unsigned int indice = faceVector[i][j];
      FSPoint p = vertexVector[indice];
      glVertex3f(p.x,p.y,p.z);
    }
    glEnd();*/
  }
  glPopMatrix();

}

unsigned int FSSurfaceMesh::convertPolygons2Triangles(void){
  vector<vector <unsigned int> > newFaceVector; //the new vector which stores the triangles instead of polygons
  
  
  /* find out number of triangles and reserve space for them */
  unsigned int numberOfNewFaces = 0;
  for(int i=0;i<faceVector.size();i++){ //iterate throught the polygons
    vector <unsigned int> polygon = faceVector[i];
    numberOfNewFaces += polygon.size()-2; //minus 2 'cos triangle has 3 vertices = 1 face
  }
  newFaceVector.reserve(numberOfNewFaces); //reserve space

  //cout << "#old: " << faceVector.size() << " #new: " << numberOfNewFaces << endl;
  
  /* decompose polygons into triangles */
  for(int i=0;i<faceVector.size();i++){ //iterate throught the polygons
    vector <unsigned int> polygon = faceVector[i];

    while(polygon.size() > 3){ //current polygon is not a triangle, so further inspection is needed
      vector <unsigned int> triangle(3,1);
      triangle.reserve(3);
      triangle[0]=polygon[0];
      triangle[2]=polygon.back();
      polygon.pop_back(); //remove last element
      triangle[1]=polygon.back();
      newFaceVector.push_back(triangle);
    }
    newFaceVector.push_back(polygon);
  }

  faceVector = newFaceVector; //assign new vector
  return 0;
}

unsigned int FSSurfaceMesh::saveToSTLFile(const char* stlFilePath){
  cout << "writing file: " << stlFilePath << endl;
  ofstream stlFile;
  stlFile.open(stlFilePath,ios::out);
  stlFile << "solid 3dscan" << endl;
  
  for(int i=0;i<faceVector.size();i++){
  
    FSPoint p1 = vertexVector[ faceVector[i][0] ];
    FSPoint p2 = vertexVector[ faceVector[i][1] ];
    FSPoint p3 = vertexVector[ faceVector[i][2] ];
    
    //generate the 3 vectors from the polygon
    FSPoint v1,v2,n;
    v1.x=p1.x-p2.x;
    v1.y=p1.y-p2.y;
    v1.z=p1.z-p2.z;

    v2.x=p2.x-p3.x;
    v2.y=p2.y-p3.y;
    v2.z=p2.z-p3.z;
        
    n.x = v1.y*v2.z - v1.z*v2.y;
    n.y = v1.z*v2.x - v1.x*v2.z;
    n.z = v1.x*v2.y - v1.y*v2.x;
    
    FSFloat d = sqrtf(n.x*n.x+n.y*n.y+n.z*n.z);

    n.x /= d;
    n.y /= d;
    n.z /= d;
  
    stlFile << "facet normal " << n.x  << " " << n.y << " " << n.z << endl;
    stlFile << "  outer loop" << endl;
    for(int j=0;j<faceVector[i].size();j++){
      unsigned int indice = faceVector[i][j];
      FSPoint p = vertexVector[indice];
      //axes are permuted to comply with 3d printing standart where the x-y plane is horizontal
        stlFile << "    vertex ";
        //stlFile << p.y*10 << " ";
        //stlFile << p.z*10+70 << " "; //all vertex coordinates must be positive (stl) so add radius of turntable
        //stlFile << p.x*10+70 << " ";
        stlFile << p.x << " ";
        stlFile << p.y << " "; //all vertex coordinates must be positive (stl) so add radius of turntable
        stlFile << p.z << " ";
        stlFile << endl;
    }
    stlFile << "  endloop" << endl;
    stlFile << "endfacet" << endl;
  }
  stlFile << "endsolid test" << endl;
  
  stlFile.close();
  return 0;
}

unsigned int FSSurfaceMesh::saveToPLYFile(const char* plyFilePath)
{
  ofstream plyFile;
  string plyFilename;
  plyFilename.assign(plyFilePath);
  //plyFilename.append("_.ply"); 
  plyFile.open(plyFilename.c_str());
  
  plyFile << "ply\n";
  plyFile << "format ascii 1.0\n";
  plyFile << "element vertex " << vertexVector.size() << "\n";;
  plyFile << "property float x\n";
  plyFile << "property float y\n";
  plyFile << "property float z\n";
  plyFile << "property uchar diffuse_red\n";
  plyFile << "property uchar diffuse_green\n";
  plyFile << "property uchar diffuse_blue\n"; 
  plyFile << "element face "<< faceVector.size() <<"\n";
  plyFile << "property list uchar int vertex_indices\n";
  plyFile << "end_header\n";

  for (int i=0; i<vertexVector.size(); i++){
    FSPoint p =  vertexVector[i];
    plyFile << p.x << " " << p.y << " " << p.z <<  " "
    << (int)p.color.red <<  " " << (int)p.color.green <<  " " << (int)p.color.blue << endl;
  }

  for (int i=0; i<faceVector.size(); i++){
    vector <unsigned int> face = faceVector[i];
    
    plyFile << face.size();
    for(int j=0;j<face.size();j++){
      plyFile << " " << face[j];
    }
    plyFile << endl;
  }
  
  plyFile.close();
  return 0;
}

unsigned int FSSurfaceMesh::saveToSCADFile(const char* scadFilePath)
{
  cout << "writing file: " << scadFilePath << endl;
  ofstream scadFile;
  scadFile.open(scadFilePath,ios::out);
  scadFile << "polyhedron(points = [" << endl;
  
  //first print all the points except last, since we dont want the ,
  for(int i=0;i<vertexVector.size()-1;i++){
    FSPoint p = vertexVector[i];
    scadFile << "["<< p.x  << ", " << p.y << ", " << p.z << "],";
  }
  //print last plus triangles label
  FSPoint p = vertexVector[vertexVector.size()-1];
  scadFile << "["<< p.x  << ", " << p.y << ", " << p.z << "]" << endl << "], " << endl << " triangles = [" ;

  for (int i=0;i<faceVector.size()-1;i++){
    scadFile << "["<< faceVector[i][0] << ", " << faceVector[i][1] << ", " << faceVector[i][2] << "],";
  }
  int i = faceVector.size()-1;
  scadFile << "["<< faceVector[i][0] << ", " << faceVector[i][1] << ", " << faceVector[i][2] << "]" << endl << "]" << endl << ");";
  
  scadFile.close();
  return 0;

}

void FSSurfaceMesh::clearAll()
{
  vertexVector.clear();
  faceVector.clear();
}