//
//  FSOpenGLView.m
//
//  Created by Francis Engelmann on 3/18/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#import "FSOpenGLView.h"

@implementation FSOpenGLView

// ----------------------------------
#pragma mark -
#pragma mark Getters and Setters
// ----------------------------------

- (void)setWorld: (FSWorld*)w {
  self->world = w;
}

- (FSWorld *)getWorld {
  return world;
}

// ----------------------------------
#pragma mark -
#pragma mark Class Lifecycle Methods
// ----------------------------------

- (id)initWithFrame:(NSRect) frameRect {

    NSLog(@"FSOpenGLView initWithFrame start");
            
    //set that this class wants to know when size of window is changed
    [self setPostsFrameChangedNotifications: YES];
    
    //create pixel format with specified attributes
    NSOpenGLPixelFormat *pixelFormat;
    NSOpenGLPixelFormatAttribute pixelFormatAttr[] =
        {
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAColorSize,
            BITS_PER_PIXEL,
            NSOpenGLPFADepthSize,
            DEPTH_SIZE,
            0.0f
        };

    pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes: pixelFormatAttr];
    //check wether pixelFormat is correctly initialized
    if(!pixelFormat){
        NSLog(@"Invalid format .. terminating.");
        return nil;
    }
    //create CocoaGL instance, using given frame and our pixelFormat
    self = [super initWithFrame: frameRect pixelFormat:pixelFormat];
    //release pixelFormat which was previously allocated
    [pixelFormat release];
    
    //check wether self was correctly initialized
    if(!self){
        NSLog(@"Self not created.. terminating");
        return nil;
    }
    
    // Now we set this context to the current context (means that its now drawable)
    [[self openGLContext] makeCurrentContext];
    
    NSLog(@"FSOpenGLView initWithFrame end");
    return self;
}

//wrapper method
- (void)prepareOpenGL {
  NSLog(@"FSOpenGLView prepareOpenGL start");
  world->init();
  NSLog(@"FSOpenGLView prepareOpenGL end");
}


- (void) reshape {
  //NSLog(@"FSOpenGLView reshape  start");
  NSSize bound = [self frame].size;    
  world->reshape(bound.width, bound.height);
  [self drawFrame];
  //NSLog(@"FSOpenGLView reshape end");
}

- (void) drawFrame {
  world->draw();
  [[self openGLContext] flushBuffer];
}

- (void) update {
  [super update];
	if (![self inLiveResize])  {// if not doing live resize
		//do some stuff
	}
  [self drawFrame];
}

// -----------------------------------------
#pragma mark - 
#pragma mark Mouse and Trackpad Event Handling
// -----------------------------------------

- (void)mouseDragged:(NSEvent *)theEvent {
  //NSLog(@"mousedragged");
  float wheelDeltaY = [theEvent deltaY]/200; //scale values
	float wheelDeltaX = [theEvent deltaX]/400; //scale values
	if (wheelDeltaY){ world->rotateY(-wheelDeltaY); }
	if (wheelDeltaX){ world->rotateX(-wheelDeltaX); }
  [self drawFrame];
}

-(void)magnifyWithEvent:(NSEvent *)event {
  FSFloat mag = [event magnification]*6; //scale value
  if (mag){ world->zoom(mag); }
  [self drawFrame];
}

/*- (void)rotateWithEvent:(NSEvent *)event {
  _float rot = [event rotation];
  NSLog(@"rotation %f", rot);
  if(rot){ view->rotate(rot); }
  [self drawFrame];
}*/

- (void)scrollWheel:(NSEvent *)theEvent
{
	float wheelDeltaY = [theEvent deltaY]/800; //scale values
	float wheelDeltaX = [theEvent deltaX]/800; //scale values
	if (wheelDeltaY){ world->rotateY(wheelDeltaY); }
	if (wheelDeltaX){ world->rotateX(wheelDeltaX); }
  [self drawFrame];
}

// -----------------------------------------
#pragma mark - 
#pragma mark IBAction
// -----------------------------------------
- (IBAction)toggleFullScreen:(id)sender {
}

@end
