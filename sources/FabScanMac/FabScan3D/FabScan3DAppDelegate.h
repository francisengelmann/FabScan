//
//  FabScan3DAppDelegate.h
//  FabScan3D
//
//  Created by Francis Engelmann on 7/1/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <QTKit/QTKit.h>

#include "FSHeaders.h"

#include "FSOpenGLView.h"
#include "FSSerial.h"
#include "FSSerial2.h"
#include "FSLaser.h"
#include "FSLight.h"
#include "FSCamera.h"
#include "FSFrame.h"
#include "FSTurnTable.h"
#include "FSVision.h"
#include "FSChessboard.h"
#include "FSBox.h"
#include "FSModel.h"
#include "FSSurfaceMesh.h"
#include "FSPointCloud.h"
#include "FSChessboardCalibration.h"
#include "FSStepper.h"

@interface FabScan3DAppDelegate : NSObject <NSApplicationDelegate> {

enum AppState
{
  AppStateStartScanning,      // when scanning process starts
  AppStateDoneScanning,       // when scanning process is finished
  AppStateInterruptScanning,  // when the scanning process gets stopped
  AppStateCalibrating,        // in the beginning when the camera is shown
  AppStateBrowsing            // when looking at the scanned model
};

@private
  NSWindow *window;
  NSWindow *controlWindow;
  FSOpenGLView* openGLView;
  NSProgressIndicator* progressIndicator;
  NSSegmentedControl* displayTypeButtons;
  NSButton* startScanButton;
  NSButton* stopScanButton;
  
  NSMenu* cameraMenu;
  NSMenu* serialPortMenu;
  
  NSInteger currSerialPortCount;  // total number of currently available serial ports
  NSString* currSerialPortPath;   // path of currently connected serial port
  
  NSInteger currCameraCount;  // total number of currently available cameras
  NSString* currCameraName;   // name of currently connected serial port
 
  
  //FSSerial* serial;
  FSSerial2* serial;
  FSLaser* laser;
  FSLight* light;
  FSTurnTable* turntable;
  
  FSStepper* stepperTurntable;
  FSStepper* stepperLaser;
  
  FSCamera* camera;
  FSWorld* world;
  FSFrame* frame;
  FSFrame* frame2;
  FSChessboard* chessboard;
  FSBox* box;
  FSModel* model;
  FSPointCloud* pointCloud;
  FSSurfaceMesh* surfaceMesh;
  
  FSChessboardCalibration* chessboardCalibration;
  FSChessboardCalibration* chessboardCalibration2;
  NSTextField *remainingTime;
  NSPopUpButton* resolutionPopUpButton;
  
  NSTextField *dpiVerticalTextField;
  NSTextField *dpiHorizontalTextField;
  NSTextField *laserStepsTextField;
  NSTextField *laserStepSizeTextField;

  NSTextField *laserCTRLStepSizeTextField;      //angle for moving laser manually with buttons
  NSTextField *turnTableCTRLStepSizeTextField;  //angle for moving turntable manually with buttons
  
  NSTextField *subLaserTresholdTextField;
  NSTextField *lowerCutOffLimitTextField;
  
  int dpiVertical;  //in pixels
  int dpiHorizontal; //in microsteps
  int laserSteps;    //integer
  FSFloat laserStepSize; //in degrees
  
  FSFloat subLaserTreshold;
  FSFloat lowerCutOffLimit;
  
  AppState state;
  unsigned char currCameraPortCount; //the number of connected cameras, used to decide wether the list should get updated or not
}

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet NSWindow *controlWindow;
@property (assign) IBOutlet FSOpenGLView *openGLView;
@property (assign) IBOutlet NSProgressIndicator *progressIndicator;
@property (assign) IBOutlet NSSegmentedControl *displayTypeButtons;
@property (assign) IBOutlet NSButton *startScanButton;
@property (assign) IBOutlet NSButton *stopScanButton;
@property (assign) IBOutlet NSPopUpButton *resolutionPopUpButton;

@property (assign) IBOutlet NSMenu *cameraMenu;
@property (assign) IBOutlet NSMenu *serialPortMenu;

@property (assign) IBOutlet NSTextField *dpiVerticalTextField;
@property (assign) IBOutlet NSTextField *dpiHorizontalTextField;
@property (assign) IBOutlet NSTextField *laserStepsTextField;
@property (assign) IBOutlet NSTextField *laserStepSizeTextField;

@property (assign) IBOutlet NSTextField *laserCTRLStepSizeTextField;
@property (assign) IBOutlet NSTextField *turnTableCTRLStepSizeTextField;


@property (assign) IBOutlet NSTextField *subLaserTresholdTextField;
@property (assign) IBOutlet NSTextField *lowerCutOffLimitTextField;


@property (assign) IBOutlet NSTextField *remainingTime;
@property (assign) IBOutlet int dpiVertical;
@property (assign) IBOutlet int dpiHorizontal;
@property (assign) IBOutlet int laserSteps;

/* Control Window Actions */

- (IBAction)turnLaserOff:(id)sender;
- (IBAction)turnLaserOn:(id)sender;
//- (IBAction)setLaserPosition:(id)sender;

- (IBAction)turnLightOff:(id)sender;
- (IBAction)turnLightOn:(id)sender;
- (IBAction)setLightIntensity:(id)sender;

- (IBAction)turntableSetEnabled:(id)sender;
- (IBAction)turntableDirectionCW:(id)sender;
- (IBAction)turntableDirectionCCW:(id)sender;

- (IBAction)laserSetEnabled:(id)sender;
- (IBAction)laserDirectionCW:(id)sender;
- (IBAction)laserDirectionCCW:(id)sender;
- (IBAction)laserTurnToInitPosition:(id)sender;
- (IBAction)laserDefineInitPosition:(id)sender;


- (IBAction)fetchFrame:(id)sender;
- (IBAction)hideFrame:(id)sender;
- (IBAction)calibrateCamera:(id)sender;
- (IBAction)positionCamera:(id)sender;
- (IBAction)autoCalibration:(id)sender;
- (IBAction)analyseFrame:(id)sender;
- (IBAction)setResolution:(id)sender;

- (IBAction)sendFabScanPing:(id)sender;
- (IBAction)clearAll:(id)sender;

- (IBAction)applyResolution:(id)sender;


/* GUI Actions */

- (IBAction)selectBrowsingType:(id)sender;

- (IBAction)startScan:(id)sender;
- (IBAction)stopScan:(id)sender;

/* Menu actions */

- (IBAction)showControlWindow:(id)sender;
- (IBAction)showPreferences:(id)sender;

- (IBAction)openPCDFile: (id)sender;
- (IBAction)savePCDFile: (id)sender;

- (IBAction)exportPTSFile: (id)sender;
- (IBAction)exportPCDFile: (id)sender;
- (IBAction)exportPLYFilePC: (id)sender;  //Point Cloud
- (IBAction)exportPLYFileSM: (id)sender;  //Surface Mesh
- (IBAction)exportSTLFile: (id)sender;
- (IBAction)exportSCADFile: (id)sender;

- (IBAction)exportAll: (id)sender;

- (IBAction)autoLaserResetAction: (id)sender;


/* Other */

- (id)init;
- (void)posCam;

- (void)continuousCheckForHardware;

/**
 * Do place the laser and camera at a position which is not measured but somehow correct, just for quick prototyping
 */
- (void)doGuessedSetup;

- (void)adaptToState;

/* Serial Port Stuff */
- (void)refreshSerialList;
- (IBAction)serialPortSelected:(id)sender;

/* Camera Stuff */
- (void) refreshCameraList;
- (IBAction)cameraSelected:(id)sender;

@end
