//
//  FSDefines.h
//  FabScan3D
//
//  Created by Francis Engelmann on 7/18/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

//zero point of laser in model is perpendicular to the chessboard plane
//the offset_angle is the angle between the perpendicular and the actual laser line
#define LASER_OFFSET_ANGLE 0.0f
#define DELAY_UNTIL_CAM_SHOT 500000


//33.2cm is the width of what the camera sees, ie place a measure tool at the back-plane and see how many cm the camera sees.
//#define FRAME_WIDTH 32.3f //32.3
#define FRAME_WIDTH 32.3f //32.3

//#define DPI_HORIZONTAL 10.0f
//#define DPI_VERTICAL 2.0f

/**
 * The following lines define the "protocol" for communication
 * between the computer and the microcontroller (MC) over the serial port.
 * Every byte stands for a operation.
 * All the values below 200 are reserved to position the servo, but servo sucks so this will be replaced and is not valid anymore
 * For 200-255 see below.
 */

#define MC_TURN_LASER_OFF      200
#define MC_TURN_LASER_ON       201
#define MC_PERFORM_STEP        202
#define MC_SET_DIRECTION_CW    203
#define MC_SET_DIRECTION_CCW   204
#define MC_TURN_STEPPER_ON     205
#define MC_TURN_STEPPER_OFF    206
#define MC_TURN_LIGHT_ON       207
#define MC_TURN_LIGHT_OFF      208
#define MC_ROTATE_LASER        209
#define MC_FABSCAN_PING        210
#define MC_FABSCAN_PONG        211
#define MC_SELECT_STEPPER      212

#define REALLY_BIG_NUMBER     99999
#define REALLY_SMALL_NUMBER  -99999

#define YES 1
#define NO  0

//treshold for substracting the laser frame from the non laser frame
#define TRESHOLD_FOR_BW 100 //70

#define STEPPER_ID_TURNTABLE  0
#define STEPPER_ID_LASER      1 

//to make the scanning process faster we ommit the lower and hight part of the cvImage
//as there is no object anyway.  The lower limit is defined by the turning table lower bound
//units are pixels, seen from the top, resp from the bottom

#define UPPER_ANALYZING_FRAME_LIMIT 250
#define LOWER_ANALYZING_FRAME_LIMIT 30

//as the actual position in the frame differs a little from calculated laserline we stop a little befor as we might catch the real non reflected laser line which creates noise
#define ANALYZING_LASER_OFFSET 100


//defining the origin in the cvFrame
//the position of intersection of back plane with ground plane in cvFrame in procent
#define ORIGIN_Y 0.725

//the angle used to scan, obsolete when the laser is also moving
#define SCANNING_ANGLE 33.69f//33.69f

//filter to remove the points below this value, used to get rid of points from the turntable surface
#define MIN_Y_COORD 0.3f

//position of laser and cam
/*#define LASER_POS_X 14.0f
#define LASER_POS_Y 5.4f
#define LASER_POS_Z 26.9f

#define CAM_POS_X -0.5f
#define CAM_POS_Y 5.06f
#define CAM_POS_Z 24.2f
*/
////position of laser and cam
#define LASER_POS_X 14.0f //precise by construction
#define LASER_POS_Y 5.4f  //not needed for calculations
#define LASER_POS_Z 28.5f //precise by construction

#define CAM_POS_X -0.7f
#define CAM_POS_Y 5.3f
#define CAM_POS_Z 27.0f

//how big is the step the laser moves when pressing the buton in the ctrl window
#define LASER_STEP 33.69f
