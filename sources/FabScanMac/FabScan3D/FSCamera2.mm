//
//  FSCamera2.m
//  FabScan
//
//  Created by Francis Engelmann on 4/8/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import "FSCamera2.h"

@implementation FSCamera2

- (id)init
{
    self = [super init];
    if (self) {
        cameras = [QTCaptureDevice inputDevicesWithMediaType: QTMediaTypeVideo];
        [self updateSelectedCameraIndex: 0];
    }
    
    return self;
}

- (IplImage*) fetchFrame
{
  
}

- (unsigned char) selectedCameraIndex
{
  return selectedCameraIndex;
}

- (void) updateSelectedCameraIndex:(unsigned char)index
{
  selectedCameraIndex = index;
  videoDevice = [cameras objectAtIndex: selectedCameraIndex];
  videoInput = [[QTCaptureDeviceInput alloc] initWithDevice:videoDevice];
}


@end
