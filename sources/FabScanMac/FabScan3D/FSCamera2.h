//
//  FSCamera2.h
//  FabScan
//
//  Created by Francis Engelmann on 4/8/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QTKit/QTKit.h>

#include "FSHeaders.h"

@interface FSCamera2 : NSObject 
{
	QTCaptureSession * captureSession;
	QTCaptureDevice * videoDevice;
	QTCaptureDeviceInput * videoInput;
  NSArray* cameras; //contains a list of all the available cameras
  unsigned char selectedCameraIndex; //the index of the currently selected camera of the "cameras"-array
  
}

- (IplImage*) fetchFrame;

- (unsigned char) selectedCameraIndex;

- (void) updateSelectedCameraIndex: (unsigned char)index;

@end
