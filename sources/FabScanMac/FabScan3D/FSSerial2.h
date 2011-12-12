//
//  FSSerial2.h
//  FabScan
//
//  Created by Francis Engelmann on 9/8/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//
//  Based on Arduino Serial Example
//  Created by Gabe Ghearing on 6/30/09.
//

#ifndef FS_SERIAL2_H
#define FS_SERIAL2_H

// import IOKit headers
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>
#include <IOKit/serial/ioss.h>
#include <sys/ioctl.h>

class FSSerial2 {
  
  private:
    int     serialFileDescriptor; // file handle to the serial port
    struct  termios gOriginalTTYAttrs; // Hold the original termios attributes so we can reset them on quit ( best practice )
    bool    readThreadRunning;
  
  public:
    
    FSSerial2(void);
    ~FSSerial2(void);
  
  
  
             void closeSerialPort();
             void closeSerialPort(int fileDescriptor);
    unsigned char openSerialPortWithBaud(const char* serialPortFile, speed_t baudRate);
    
             void writeByte(char byte);
    unsigned char readByte(void);
};
#endif