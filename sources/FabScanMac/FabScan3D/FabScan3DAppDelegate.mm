//
//  FabScan3DAppDelegate.m
//  FabScan
//
//  Created by Francis Engelmann on 7/1/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#import "FabScan3DAppDelegate.h"

#include "FSGeometry.h"

@implementation FabScan3DAppDelegate

@synthesize dpiVertical;
@synthesize dpiHorizontal;
@synthesize laserSteps;
@synthesize window;
@synthesize controlWindow;
@synthesize openGLView;
@synthesize remainingTime;
@synthesize progressIndicator;
@synthesize displayTypeButtons;
@synthesize cameraMenu;
@synthesize serialPortMenu;
@synthesize startScanButton;
@synthesize stopScanButton;
@synthesize resolutionPopUpButton;

@synthesize dpiVerticalTextField;
@synthesize dpiHorizontalTextField;
@synthesize laserStepsTextField;
@synthesize laserStepSizeTextField;
@synthesize lowerCutOffLimitTextField;
@synthesize subLaserTresholdTextField;

@synthesize laserCTRLStepSizeTextField;
@synthesize turnTableCTRLStepSizeTextField;

// ---------------------------------------------
#pragma mark -
#pragma mark Object Lifeycle Methods
// ---------------------------------------------

- (id) init
{
  if((self = [super init])){
    NSLog(@"%s",__PRETTY_FUNCTION__);
    
    [progressIndicator setDisplayedWhenStopped: NO];
    
    NSString *resourcesPath = [[NSBundle mainBundle] resourcePath];
      //model->convertPTS2OFF([resourcesPath UTF8String]);
    
    
    //setting initial values, can be changed in ctrl panel
    dpiHorizontal = 40;
    dpiVertical = 15;
    laserSteps = 1;
    laserStepSize = 3;
    
    subLaserTreshold = 70;
    lowerCutOffLimit = 0.2f;
    
    currSerialPortCount = 0;
    currSerialPortPath = @"";
    
    chessboardCalibration = new FSChessboardCalibration([resourcesPath UTF8String]);
    chessboardCalibration2 = new FSChessboardCalibration([resourcesPath UTF8String]);
    //serial = new FSSerial(9600);
    serial = new FSSerial2();
    world = new FSWorld();
    light = new FSLight(serial);

    stepperTurntable = new FSStepper(serial, STEPPER_ID_TURNTABLE);
    stepperLaser = new FSStepper(serial, STEPPER_ID_LASER);
    
    //laser->turnOn();
    laser = new FSLaser(stepperLaser,serial,170);    
    //laser->setServoAngle(33.69f); //this is the angle when the laser points a the center of the turntable
//    laser->setServoAngle(0.0);
    laser->setShouldDisplay(0);
    
    turntable = new FSTurnTable(stepperTurntable);
    camera = new FSCamera([resourcesPath UTF8String]);
    frame = new FSFrame();    
    
    //frame2 = new FSFrame(FSMakePoint(-35.0f,0.0f,0.0f));
    //frame2->setSize(FSMakeSize(10, 10, 0.0f));
    
    pointCloud = new FSPointCloud();
    pointCloud->setTurnTable(turntable);
    surfaceMesh = new FSSurfaceMesh();
    
    model = new FSModel(pointCloud, surfaceMesh);
    
    world->setModel(model);
    
    chessboard = new FSChessboard();
    box = new FSBox();
    
    world->addObject(camera);
    world->addObject(turntable);
    world->addObject(frame);
    //world->addObject(frame2);
    world->addObject(pointCloud);
    world->addObject(surfaceMesh);
    world->addObject(laser);
    //world->addObject(chessboard);
    //world->addObject(box);
    
    [progressIndicator setUsesThreadedAnimation: YES];
    
    //[window makeMainWindow];
    [window makeKeyWindow];
    [openGLView drawFrame];
  }
  return self;
}

- (void) awakeFromNib {
  
  if (system(NULL)){
    [startScanButton setEnabled: NO];
  }
  [openGLView setWorld: world];
  [self doGuessedSetup];
  [openGLView drawFrame];
  NSLog(@"AWAKE FROM NIB");
  
  [NSThread detachNewThreadSelector:@selector(continuousCheckForHardware) toTarget:self withObject:nil];
  
  //[self performSelector:@selector(performHardwareSetup:) withObject:nil afterDelay:5.0]; 

  state = AppStateCalibrating;
  [self adaptToState];
}

//called every few seconds to check wether new hardware is connected
- (void)continuousCheckForHardware
{
  NSAutoreleasePool *innerPool = [[NSAutoreleasePool alloc] init];
  while(YES){
    sleep(1);
   // NSLog(@"%s",__PRETTY_FUNCTION__);
    [self refreshSerialList];
    [self refreshCameraList];
    //[NSTimer scheduledTimerWithTimeInterval: 1.0
    //target:self
    //selector:@selector(continuousCheckForHardware)
    //userInfo:nil
    //repeats: YES];
  }
  [innerPool release];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
  return YES;
}

-(NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender
{
  NSLog(@"%s",__PRETTY_FUNCTION__);
  
  laser->turnToInitPosition();
  
  //delete frame;
	//delete laser;
  
  //delete serial;
	delete serial;
	//delete camera;
  //delete chessboard;
  //delete box;
  //delete pointCloud;
  
  return NSTerminateNow;
}

// ---------------------------------------------
#pragma mark -
#pragma mark GUI IBActions
// ---------------------------------------------

- (IBAction)startScan:(id)sender
{
  state = AppStateStartScanning;
  [self adaptToState];
  [NSThread detachNewThreadSelector:@selector(analyseFrame:)
                         toTarget: self
                       withObject: nil];
}

- (IBAction)stopScan:(id)sender
{
  state = AppStateInterruptScanning;
  [self adaptToState];
  
  //turntable->turnNumberOfDegrees(-(turntable->getRotation()).y);
}

- (IBAction)showControlWindow:(id)sender
{
  [controlWindow orderFrontRegardless];
}

- (IBAction)showPreferences:(id)sender
{
  NSRect winframe = [window frame];
  winframe.size.height += 100; 
  [window setFrame: winframe display:YES animate:YES];
}


- (IBAction)selectBrowsingType:(id)sender
{
  //NSBundle* mainBundle;
  //mainBundle = [NSBundle mainBundle];
  //NSBundle* myBundle = [NSBundle mainBundle];
  //NSString* myImage = [myBundle pathForResource:@"Seagull" ofType:@"jpg"];


  if([sender selectedSegment]==0){
    pointCloud->setShouldDisplay(1);
    surfaceMesh->setShouldDisplay(0);
    [openGLView drawFrame];
  }else{
    if(model->getSurfaceMeshGenerated() == NO){
      NSString *resourcesPath = [[NSBundle mainBundle] resourcePath];
      model->convertPTS2OFF([resourcesPath UTF8String]);
      model->setSurfaceMeshGenerated(YES);
    }else if(model->getSurfaceMeshLoaded() == NO){
      surfaceMesh->openFromOFFFile("pc.off");
      model->setSurfaceMeshLoaded(YES);
    }
    
    if(model->getPolygons2TrianglesTransformed() == NO){
      surfaceMesh->convertPolygons2Triangles();
      model->setPolygons2TrianglesTransformed(YES);
    }

    pointCloud->setShouldDisplay(0);
    surfaceMesh->setShouldDisplay(1);
    [openGLView drawFrame];
  }
}

// ---------------------------------------------
#pragma mark -
#pragma mark Control IBActions
// ---------------------------------------------

/* Light Controls */

- (IBAction)turnLightOff:(id)sender
{
  light->turnOff();//serial->writeChar(TURN_LIGHT_OFF);
}

- (IBAction)turnLightOn:(id)sender
{
  light->turnOn();
}

- (IBAction)setLightIntensity:(id)sender
{
  light->turnOn([sender intValue]);
}

/* Laser Controls */

- (IBAction)turnLaserOff:(id)sender {
  laser->turnOff();
  [openGLView drawFrame];
}

- (IBAction)turnLaserOn:(id)sender {
  laser->turnOn();
  [openGLView drawFrame];
}

/* TurnTable Stepper Controls*/

- (IBAction)turntableSetEnabled:(id)sender
{
  if([sender state]==NSOnState){
    NSLog(@"nsOnState");
    turntable->enable();
  }else if([sender state]==NSOffState){
    NSLog(@"nsOffState");
    turntable->disable();
  }
  [openGLView drawFrame];
}

- (IBAction)turntableDirectionCW:(id)sender {
  turntable->setDirection(FS_DIRECTION_CW);
  usleep(50000);
  turntable->turnNumberOfDegrees([turnTableCTRLStepSizeTextField floatValue]);
  usleep(50000);
  [openGLView drawFrame];
  FSPoint alphaDelta = turntable->getRotation();
  printf("%s %f \n", __PRETTY_FUNCTION__, alphaDelta.y);
}

- (IBAction)turntableDirectionCCW:(id)sender
{
  turntable->setDirection(FS_DIRECTION_CCW);
  usleep(50000);
  turntable->turnNumberOfDegrees([turnTableCTRLStepSizeTextField floatValue]);
  usleep(50000);
  [openGLView drawFrame];
  FSPoint alphaDelta = turntable->getRotation();
  printf("%s %f \n", __PRETTY_FUNCTION__, alphaDelta.y);
}


/* Laser Stepper Controls*/

- (IBAction)laserSetEnabled:(id)sender
{
  if([sender state]==NSOnState){
    NSLog(@"nsOnState");
    laser ->enable();
  }else if([sender state]==NSOffState){
    NSLog(@"nsOffState");
    laser->disable();
  }
  [openGLView drawFrame];
}

- (IBAction)laserDirectionCW:(id)sender {
  laser->setDirection(FS_DIRECTION_CW);
  usleep(50000);
  int steps = (int)([laserCTRLStepSizeTextField floatValue]*200.0f*16.0f/360.0f);
  FSFloat degrees = steps/200.0f/16.0f*360.0f;
  laser->turnNumberOfDegrees(degrees);  
  [laserCTRLStepSizeTextField setFloatValue: degrees];
  usleep(50000);
  [openGLView drawFrame];
  FSPoint alphaDelta = laser->getRotation();
  printf("%s %f \n", __PRETTY_FUNCTION__, alphaDelta.y);
}

- (IBAction)laserDirectionCCW:(id)sender
{
  laser->setDirection(FS_DIRECTION_CCW);
  usleep(50000);
  int steps = (int)([laserCTRLStepSizeTextField floatValue]*200.0f*16.0f/360.0f);
  FSFloat degrees = steps/200.0f/16.0f*360.0f;
  laser->turnNumberOfDegrees(degrees);  
  [laserCTRLStepSizeTextField setFloatValue: degrees];
  usleep(50000);
  [openGLView drawFrame];
  FSPoint alphaDelta = laser->getRotation();
  printf("%s %f \n", __PRETTY_FUNCTION__, alphaDelta.y);
}

- (IBAction)laserTurnToInitPosition:(id)sender
{
  FSPoint rotation = laser->getRotation();
  if(rotation.y > 0){
      laser->setDirection(FS_DIRECTION_CW);
      usleep(5000);
      laser->turnNumberOfDegrees(rotation.y);
    }else{
      laser->setDirection(FS_DIRECTION_CCW);
      usleep(5000);
      laser->turnNumberOfDegrees(-rotation.y);
    }

}

- (IBAction)laserDefineInitPosition:(id)sender
{
  laser->setRotation(FSMakePoint(0.0f, 0.0f, 0.0f));
}


/* Misc Controls */

- (IBAction)clearAll:(id)sender
{
  model->close();
  [openGLView drawFrame];
}

//this is the action from the control window
- (IBAction)applyResolution:(id)sender
{
  NSLog(@"%s",__PRETTY_FUNCTION__);
  dpiVertical = [dpiVerticalTextField intValue];
  dpiHorizontal = [dpiHorizontalTextField intValue];
  laserSteps = [laserStepsTextField intValue];
  laserStepSize = [laserStepSizeTextField floatValue];
  
  subLaserTreshold = [subLaserTresholdTextField floatValue];
  lowerCutOffLimit = [lowerCutOffLimitTextField floatValue];
  
  NSLog(@"%s %d %d %d %f",__PRETTY_FUNCTION__,dpiVertical,dpiHorizontal,laserSteps,laserStepSize);
  
}


- (IBAction)setResolution:(id)sender
{
  switch([sender indexOfSelectedItem]){
    case 0: //lower
      dpiVertical = 80;
      dpiHorizontal = 160;
      break;
    case 1: //low
      dpiVertical = 40;
      dpiHorizontal = 80;
    case 2://normal
      dpiHorizontal = 40;
      dpiVertical = 15;
      laserSteps = 1;
      laserStepSize = 3;
    case 3://hight
      dpiVertical = 10;
      dpiHorizontal = 20;
      break;
    case 4://higher
      dpiHorizontal = 80;
      dpiVertical = 3;
      laserSteps = 3;
      laserStepSize = 2;
      break;
    case 5://maximum
      dpiVertical = 1;
      dpiHorizontal = 1;
      break;
    default:
      break;
  }
}


- (IBAction)sendFabScanPing:(id)sender
{
  serial->writeByte(MC_FABSCAN_PING);
  NSLog(@"fabscan pong %d",serial->readByte());
}

/*- (IBAction)setLaserPosition:(id)sender {
  //laser->turnOn();
  laser->setServoAngle([sender maxValue]-[sender intValue]);
  
  
  //usleep(500000);
  IplImage* frameWithLaser = camera->fetchFrame();
  FSVision::addGreenLineTo(frameWithLaser);
  FSVision::drawLaserLineFromLaserToImageSeenFromCamera(laser,frameWithLaser,camera);

  //frame->setFrame(frameWithLaser);
  
  /*usleep(20000);
  laser->turnOff();
  IplImage* frameWithoutLaser = camera->fetchFrame();
  frame->setFrame(FSVision::subLaser(frameWithLaser,frameWithoutLaser));
  
  //frame->setFrame(cvCloneImage(camera->fetchQuickFrame()));
  [openGLView drawFrame];
}*/

- (IBAction)fetchFrame:(id)sender {
  //laser->turnOn();
  //usleep(500000);
  //IplImage* f = camera->fetchFrame();
  //FSVision::addGreenLineTo(f);
  //frame2->setFrame(f);
  
  IplImage* uf = camera->fetchFrame();
  FSVision::addGreenLineTo(uf);
  FSVision::drawLaserLineFromLaserToImageSeenFromCamera(laser,uf,camera);

  frame->setFrame(uf);
  
  [openGLView drawFrame];

  //cvReleaseImage(&f);
  //cvReleaseImage(&uf);

}

- (IBAction)hideFrame:(id)sender
{
  frame->setFrame(NULL);
  [openGLView drawFrame];
}

/* Camera Controls */

- (IBAction)calibrateCamera:(id)sender
{
  if(!chessboardCalibration->needMoreBoardsForCalibration()){
    NSLog(@"%s enough data, calibrating now",__PRETTY_FUNCTION__);
    //IplImage* f = camera->fetchFrame();
    //frame2->setFrame(chessboardCalibration->calibrateCamera(f));
    FSPoint camPosition;
    camPosition = chessboardCalibration->calibrateCamera();
    camera->setPosition(camPosition);
    
    camera->setIntrinsic(chessboardCalibration->getInstrinsicMatrix());
    camera->setDistortion(chessboardCalibration->getDistortionCoeffs());
    [openGLView drawFrame];
  }else{
    IplImage* calibImage = camera->fetchFrame();
    IplImage* frameWithMarkers = cvCloneImage(calibImage);
    int foundCompleteChessboard = chessboardCalibration->findBoardInFrame(frameWithMarkers);
    frame->setFrame(frameWithMarkers);
    [openGLView drawFrame];
    if(foundCompleteChessboard>0){
      char* name;
      asprintf(&name,"calib%d.jpg",foundCompleteChessboard);
      cvSaveImage(name, calibImage);
    }
  }
}

- (IBAction)positionCamera:(id)sender
{
  NSTimer * timer = [NSTimer scheduledTimerWithTimeInterval:0.3f target:self selector:@selector(posCam) userInfo:nil repeats:YES];
  NSRunLoop *runner = [NSRunLoop currentRunLoop];
  [runner addTimer: timer forMode: NSDefaultRunLoopMode];
}

- (IBAction)autoCalibration:(id)sender {
  if (chessboardCalibration->autoCalibrate()){
    FSPoint camPosition;
    camPosition = chessboardCalibration->calibrateCamera();
    camera->setPosition(camPosition);
    
    camera->setIntrinsic(chessboardCalibration->getInstrinsicMatrix());
    camera->setDistortion(chessboardCalibration->getDistortionCoeffs()); 
  }
}

// ---------------------------------------------
#pragma mark -
#pragma mark Menu IBActions
// ---------------------------------------------

- (IBAction)openPCDFile: (id)sender {
  NSArray *fileTypes = [NSArray arrayWithObject:@"pcd"];
  NSOpenPanel *op = [NSOpenPanel openPanel];
  [op setAllowedFileTypes: fileTypes];
  if ([op runModal] == NSOKButton){
    NSString *filename = [op filename];
    pointCloud->clearAll();
    pointCloud->openFromPCDFile( [filename UTF8String] );
    state = AppStateBrowsing;
    [displayTypeButtons setSelectedSegment: 0];
    pointCloud->setShouldDisplay(1);
    surfaceMesh->setShouldDisplay(0);
    model->setSurfaceMeshGenerated(NO);
    model->setSurfaceMeshLoaded(NO);
    [self adaptToState];
  }
  [openGLView drawFrame];
}

- (IBAction)savePCDFile: (id)sender {
  //NSArray *fileTypes = [NSArray arrayWithObject:@"pcd"];
  NSSavePanel *save = [NSSavePanel savePanel];
  //[save setAllowedFileTypes: fileTypes];
  int result = [save runModal];
  if (result == NSOKButton){
    NSString *selectedFile = [save filename];
    pointCloud->saveToPLYFile( [selectedFile UTF8String] );
  }
}

- (IBAction)exportPTSFile: (id)sender
{
  NSArray *fileTypes = [NSArray arrayWithObject:@"pts"];
  NSSavePanel *save = [NSSavePanel savePanel];
  [save setAllowedFileTypes: fileTypes];
  int result = [save runModal];
  if (result == NSOKButton){
    NSString *selectedFile = [save filename];
    pointCloud->saveToPTSFile( [selectedFile UTF8String] );
  }
}

- (IBAction)exportPCDFile: (id)sender
{
  NSArray *fileTypes = [NSArray arrayWithObject:@"pcd"];
  NSSavePanel *save = [NSSavePanel savePanel];
  [save setAllowedFileTypes: fileTypes];
  int result = [save runModal];
  if (result == NSOKButton){
    NSString *selectedFile = [save filename];
    pointCloud->saveToPCDFile( [selectedFile UTF8String] );
  }
}

- (IBAction)exportPLYFile: (id)sender
{
  NSArray *fileTypes = [NSArray arrayWithObject:@"ply"];
  NSSavePanel *save = [NSSavePanel savePanel];
  [save setAllowedFileTypes: fileTypes];
  int result = [save runModal];
  if (result == NSOKButton){
    NSString *selectedFile = [save filename];
    pointCloud->saveToPLYFile( [selectedFile UTF8String] );
  }
}

- (IBAction)exportSTLFile: (id)sender
{
  NSArray *fileTypes = [NSArray arrayWithObject:@"stl"];
  NSSavePanel *save = [NSSavePanel savePanel];
  [save setAllowedFileTypes: fileTypes];
  int result = [save runModal];
  if (result == NSOKButton){
    NSString *selectedFile = [save filename];
    if(model->getPolygons2TrianglesTransformed() == NO)
    {
      surfaceMesh->convertPolygons2Triangles();
      model->setPolygons2TrianglesTransformed(YES);
    }
    surfaceMesh->saveToSTLFile( [selectedFile UTF8String] );
  }
}

- (IBAction)exportAll: (id)sender
{
  //NSArray *fileTypes = [NSArray arrayWithObject:@"stl"];
  NSSavePanel *save = [NSSavePanel savePanel];
  //[save setAllowedFileTypes: fileTypes];
  int result = [save runModal];
  if (result == NSOKButton){
    NSString *selectedFile = [save filename];
    
    //stl specific

    
    if(model->getPolygons2TrianglesTransformed() == NO){
      surfaceMesh->convertPolygons2Triangles();
      model->setPolygons2TrianglesTransformed(YES);
    }
    surfaceMesh->saveToSTLFile([[selectedFile stringByAppendingString: @".stl"] UTF8String] );
    pointCloud->saveToPCDFile( [[selectedFile stringByAppendingString: @".pcd"] UTF8String] );
    pointCloud->saveToPTSFile( [[selectedFile stringByAppendingString: @".pts"] UTF8String] );
    pointCloud->saveToPLYFile( [[selectedFile stringByAppendingString: @".ply"] UTF8String] );
  }
}

// ---------------------------------------------
#pragma mark -
#pragma mark Core Functionality
// ---------------------------------------------


//runs in a thread
- (IBAction)analyseFrame:(id)sender {
  FSFloat stepDegrees = dpiHorizontal/200.0f/16.0f*360.0f;
;
  NSAutoreleasePool *innerPool = [[NSAutoreleasePool alloc] init];
  
  NSLog(@"%s",__PRETTY_FUNCTION__);
//  laser->setServoAngle(0.0f);

  laser->turnOff();
  usleep(5000);
  light->turnOn(127);
  usleep(50000);
  laser->enable();
  usleep(5000);
  laser->setDirection(FS_DIRECTION_CCW);
  usleep(5000);
  FSPoint rot = laser->getRotation();
  laser->turnNumberOfDegrees(SCANNING_ANGLE-rot.y+(laserSteps-1)*laserStepSize/2);
  usleep(2000000); //wait a little linge since the laser needs time to turn into position
  
  turntable->setDirection(FS_DIRECTION_CCW);
  turntable->enable();
  serial->writeByte(MC_SET_DIRECTION_CCW);  
  
  NSDate *date1 = [NSDate date];
  [progressIndicator setIndeterminate: NO];

  [progressIndicator setHidden: NO];
  [progressIndicator startAnimation: self];
  //[progressIndicator setIndeterminate: YES];
  
    for(FSFloat i=0; i<360 && state==AppStateStartScanning; i+=stepDegrees){
      laser->turnOff();
      [openGLView performSelectorOnMainThread:@selector(drawFrame) withObject:nil waitUntilDone:false];
      usleep(DELAY_UNTIL_CAM_SHOT);
      IplImage* noLaserFrame = camera->fetchFrame();
      //frame->setFrame(cvCloneImage( noLaserFrame));
      //NSLog(@" %s 1 \n",__PRETTY_FUNCTION__);

      laser->toggleDirection();
      laser->turnOn();
      [openGLView performSelectorOnMainThread:@selector(drawFrame) withObject:nil waitUntilDone:false];
      usleep(DELAY_UNTIL_CAM_SHOT);
      
      //this is the first iteration over the laser steps
      IplImage* laserFrame = camera->fetchFrame();
      FSVision::putPointsFromFrameToCloud(noLaserFrame,laserFrame,pointCloud,laser,camera,turntable, dpiVertical, lowerCutOffLimit, subLaserTreshold);
      cvReleaseImage(&laserFrame);
      //here are the others
      for(int t=1; t<laserSteps && state==AppStateStartScanning; t++){
        printf("t: %d laserSteps: %d \n",t,laserSteps);
        laser->turnNumberOfDegrees(laserStepSize);
        [openGLView performSelectorOnMainThread:@selector(drawFrame) withObject:nil waitUntilDone:false];
        usleep(DELAY_UNTIL_CAM_SHOT);
        usleep(DELAY_UNTIL_CAM_SHOT);
        IplImage* laserFrame = camera->fetchFrame();
        FSVision::putPointsFromFrameToCloud(noLaserFrame,laserFrame,pointCloud,laser,camera,turntable, dpiVertical, lowerCutOffLimit, subLaserTreshold);
        cvReleaseImage(&laserFrame);
      }

      //NSLog(@" %s 3 \n",__PRETTY_FUNCTION__);
      
      turntable->turnNumberOfDegrees(stepDegrees);
      [openGLView performSelectorOnMainThread:@selector(drawFrame) withObject:nil waitUntilDone:false];
      usleep(DELAY_UNTIL_CAM_SHOT);
      usleep(DELAY_UNTIL_CAM_SHOT);      //NSLog(@" %s 4 \n",__PRETTY_FUNCTION__);
      
      //[openGLView drawFrame];
    
      [progressIndicator setDoubleValue: i];
    
      /* calculate remaining time */
      NSDate *date2 = [NSDate date];
      NSTimeInterval currentSteps = [date2 timeIntervalSinceDate:date1]; //double the time it took to perform i steps
      NSTimeInterval allSteps = currentSteps/i*360.0; //the estimated time of one rotation
      NSTimeInterval remainingSteps = allSteps - currentSteps; //time for remaining steps
      
      int secs = (int)remainingSteps % 60;
      int mins = (int)remainingSteps / 60;

      NSString* minsStr;
      NSString* secsStr;
      
      if(secs<10){
        secsStr = [NSString stringWithFormat:@"0%d",secs];
      }else{
        secsStr = [NSString stringWithFormat:@"%d",secs];
      }

      if(mins <60){
        minsStr = [NSString stringWithFormat:@"0%d",mins];
      }else{
        minsStr = [NSString stringWithFormat:@"%d",mins];
      }

      if(i>1){
        NSString* remainingTimeString = [NSString stringWithFormat: @" %@:%@ ",minsStr,secsStr];
        [remainingTime setStringValue: remainingTimeString];
        [progressIndicator setIndeterminate: NO];
      }
      
      
      //NSLog(@"%s %d:%d",__PRETTY_FUNCTION__,(int)remainingSteps/60,(int)remainingSteps%60);
      
      //[openGLView performSelectorOnMainThread:@selector(updateRemainingTime) withObject:remainingSteps waitUntilDone:false];
      
      cvReleaseImage(&noLaserFrame);
    }
  [progressIndicator stopAnimation: sender];
  [openGLView performSelectorOnMainThread:@selector(drawFrame) withObject:nil waitUntilDone:false];
  
  state = AppStateDoneScanning;
//  [self adaptToState];
  [self performSelectorOnMainThread:@selector(adaptToState) withObject:nil waitUntilDone:true];

  laser->turnToInitPosition();

  //camera->setTarget(FSMakePoint(0.0f, <#FSFloat y#>, 7.3f));
  
  [innerPool release];
}

- (void)posCam
{
  NSLog(@"%s fire",__PRETTY_FUNCTION__);
  IplImage* blub = camera->setPositionUsingChessboardCalibration(chessboardCalibration);
  //IplImage* blub = camera->fetchUndistortedFrame();
  FSVision::addGreenLineTo(blub);
  frame->setFrame(blub);
  [openGLView drawFrame];
}

- (void)doGuessedSetup
{
  laser->setPosition( FSMakePoint(LASER_POS_X,LASER_POS_Y,LASER_POS_Z) ); //28.5
  FSPoint camPos = FSMakePoint(CAM_POS_X, CAM_POS_Y, CAM_POS_Z); //26.37
  FSPoint camRot = FSMakePoint(0.0f, 0.0f, 0.0f); //-1.5
  camera->setPosition(camPos);
  camera->setRotation(camRot);
  
  FSSize imgSize = FSMakeSize(FRAME_WIDTH, FRAME_WIDTH*(3.0f/4.0f), 0.0f);
  frame->setSize(imgSize);
  FSPoint imgPos;
  imgPos.x = -imgSize.width/2.0f;
  float angle = camRot.x*((float)M_PI/180.0f);
  printf("%s cam angle %f %f\n",__PRETTY_FUNCTION__, angle, tan(angle));
  imgPos.y = camPos.y - imgSize.height/2.0f + (float)tan(angle)*camPos.z;
  imgPos.z = 0.0f;
  frame->setPosition(imgPos);
  //frame->setPosition(FSMakePoint(-15.5f,-8.3f,0.0f));
}

- (void)adaptToState
{
  switch(state){
    case AppStateBrowsing:
      [startScanButton setHidden: NO];
      [stopScanButton setHidden: YES];
      laser->turnOff();
      turntable->disable();
      [progressIndicator setHidden:YES];
      [remainingTime setHidden:YES];
      [remainingTime setStringValue: @"calculating remaining time..."];
      [displayTypeButtons setHidden: NO];
      break;
      
    case AppStateCalibrating:
      [startScanButton setHidden: NO];
      [stopScanButton setHidden: YES];
      laser->turnOn();
      turntable->enable();
      break;
      
    case AppStateStartScanning:
      [progressIndicator setHidden:NO];
      [startScanButton setHidden: YES];
      [stopScanButton setHidden: NO];
      [window makeFirstResponder: stopScanButton];
      [displayTypeButtons setHidden: YES];
      [remainingTime setHidden: NO];
      [resolutionPopUpButton setEnabled: NO];
      break;

    case AppStateDoneScanning:
      NSLog(@"done scanning");
      [startScanButton setHidden: NO];
      [stopScanButton setHidden: YES];
      [window makeFirstResponder: startScanButton];
      [progressIndicator setHidden:YES];
      [remainingTime setHidden: YES];
      [displayTypeButtons setHidden: NO];
      [resolutionPopUpButton setEnabled: YES];
      turntable->disable();  
      laser->turnOff();
      [[NSSound soundNamed:@"Glass"] play];
      state=AppStateBrowsing;
      [self adaptToState];
      break;

    case AppStateInterruptScanning:
      [resolutionPopUpButton setEnabled: YES];
      [startScanButton setHidden: NO];
      [stopScanButton setHidden: YES];
      [window makeFirstResponder: startScanButton];
      state=AppStateBrowsing;
      [self adaptToState];
      break;
  }
}

// ---------------------------------------------
#pragma mark -
#pragma mark Serial Port Stuff
// ---------------------------------------------

/*- (BOOL)validateMenuItem:(NSMenuItem *)item
{
  //if(serial2->openSerialPortWithBaud([[item title] cStringUsingEncoding:NSUTF8StringEncoding], 9600)){
  //  return NO;
  //}else{
  //  return YES;
  
  //NSLog(@"%s",__PRETTY_FUNCTION__);
  [self refreshSerialList];
  return YES;
}*/

//- (void) refreshSerialList: (NSString *) selectedText {
- (void) refreshSerialList
{
  //NSLog(@"%s",__PRETTY_FUNCTION__);

  //NSLog(@"%s",__PRETTY_FUNCTION__);
	io_object_t serialPort;
	io_iterator_t serialPortIterator;
	
	// remove everything from the menu
	[serialPortMenu removeAllItems];
  
	// ask for all the serial ports
	IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceMatching(kIOSerialBSDServiceValue), &serialPortIterator);
	
	// loop through all the serial ports and add them to the array
	int i = 0;
  while ((serialPort = IOIteratorNext(serialPortIterator))) {
    i++;
    NSMenuItem* newItem = [[NSMenuItem allocWithZone:[NSMenu menuZone]]
                                              initWithTitle: (NSString*)IORegistryEntryCreateCFProperty(serialPort, CFSTR(kIOCalloutDeviceKey), kCFAllocatorDefault, 0)
                                              action: @selector (serialPortSelected:)
                                              keyEquivalent:@""];
    [newItem setTag: i];
    [newItem setTarget: self];

    if([[newItem title] isEqualToString: currSerialPortPath]){
      [newItem setState: NSOnState];
    }

    [serialPortMenu addItem: newItem];		
		IOObjectRelease(serialPort);
	}
	
  if(i!=currSerialPortCount){
    NSLog(@"Number of available serial ports changed.");
    if(i>currSerialPortCount || (i==currSerialPortCount && ![currSerialPortPath isEqualToString:[[serialPortMenu itemAtIndex: 0] title]]) ){ //a new serial port became available, probably FabScan connected OR the user switch the connection so fast that we did not catch it, but than the name has changed
      unsigned char res = serial->openSerialPortWithBaud([[[serialPortMenu itemAtIndex: 0] title] cStringUsingEncoding:NSUTF8StringEncoding], 9600);
      if(res==0){ //no error while opening selected serial port
        currSerialPortPath = [NSString stringWithString: [[serialPortMenu itemAtIndex: 0] title]];
        NSLog(@"open!!!! %u",res);
        [window makeFirstResponder: startScanButton];
        [startScanButton setEnabled: YES];
      }else{
        //currSerialPortPath = [NSString stringWithString: @"a"];
        NSLog(@"fail!!!! %u",res);
      }
    }else if(i<currSerialPortCount){ //a serial port left, probably fabscan disconncected
      if( ![currSerialPortPath isEqualToString: @""]){
        currSerialPortPath = [NSString stringWithFormat: @""];
        serial->closeSerialPort();
      }
    }
    currSerialPortCount=i;    
  }
  
	//add the selected text to the top
	//[serialPortMenu insertItemWithTitle:selectedText atIndex:0];
	//[serialPortMenu selectItemAtIndex:0];
	
	IOObjectRelease(serialPortIterator);
}

//called when a serial port is selected from the menu
- (IBAction)serialPortSelected:(id)sender
{
  unsigned char res = serial->openSerialPortWithBaud([[sender title] cStringUsingEncoding:NSUTF8StringEncoding], 9600);
  
  if(res==0){ //no error while opening selected serial port
    [sender setState: NSOnState];
    currSerialPortPath = [NSString stringWithString: [sender title]];
    NSLog(@"open!!!! %u",res);    
    [startScanButton setEnabled: YES];
    [window makeFirstResponder: startScanButton];
  }else{
    NSLog(@"fail!!!! %u",res);
  }
}


// ---------------------------------------------
#pragma mark -
#pragma mark Camera Stuff
// ---------------------------------------------

- (void) refreshCameraList
{
  NSArray* camArray = [QTCaptureDevice inputDevicesWithMediaType: QTMediaTypeVideo];
  //NSLog(@"cam count %d",[camArray count]);
  
  [cameraMenu removeAllItems];
  NSInteger i = 0;

  
  for (QTCaptureDevice* cam in camArray) {
    //NSString* camName;
     
     if([cam attributeForKey: QTCaptureDeviceInputSourceIdentifierKey]==nil)
     {
      //NSLog(@"%d is nil",i);
     }else{
      //NSLog(@"%d is not nil",i);
     }

  
    NSMenuItem* newItem = [[NSMenuItem allocWithZone:[NSMenu menuZone]]
                                              initWithTitle: @"cam"
                                              action: @selector (cameraSelected:)
                                              keyEquivalent: @""];
    [newItem setTag: i];
        i++;

    [newItem setTarget: self];
    
    if([cam isOpen]){
      [newItem setState: NSOnState];
    }

    [cameraMenu addItem: newItem];	
  }
}

- (IBAction)cameraSelected:(id)sender
{
  QTCaptureDevice* cam = [[QTCaptureDevice inputDevicesWithMediaType: QTMediaTypeVideo] objectAtIndex: [sender tag]];
  [cam open: nil]; //nil cos we dont care about errors :P
}

@end
