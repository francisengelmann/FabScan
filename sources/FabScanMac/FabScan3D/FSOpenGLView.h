//
//  FSOpenGLView.h
//
//  Created by Francis Engelmann on 3/18/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#import  <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>

#include "FSHeaders.h"

#include "FSWorld.h"

#define BITS_PER_PIXEL          32.0f
#define DEPTH_SIZE              32.0f
#define DEFAULT_TIME_INTERVAL   (1.0f/60.0f)

@interface FSOpenGLView : NSOpenGLView {
    bool FullScreenOn;
    bool first;
    
    NSWindow *FullScreenWindow;
    NSWindow *StartingWindow;
    NSTimer  *time;
    
    FSWorld* world;
}

//Getters and Setters

- (void)setWorld:(FSWorld*) world;
- (FSWorld*)getWorld;

//Actions

- (IBAction)toggleFullScreen:(id)sender;

- (void) prepareOpenGL;
- (void) update;
- (void) reshape;
- (void) drawFrame;

- (id) initWithFrame: (NSRect) frameRect;

@end
