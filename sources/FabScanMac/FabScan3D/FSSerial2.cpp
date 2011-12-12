//
//  FSSerial2.cpp
//  FabScan3D
//
//  Created by Francis Engelmann on 9/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "FSSerial2.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <paths.h>
#include <termios.h>
#include <sysexits.h>
#include <sys/param.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <AvailabilityMacros.h>

FSSerial2::FSSerial2(){
  serialFileDescriptor = -1;
  readThreadRunning = FALSE;
}

FSSerial2::~FSSerial2()
{
  printf("%s destructor\n",__PRETTY_FUNCTION__);
  this->closeSerialPort(serialFileDescriptor);
}

// open the serial port
//   - 0 is returned on success
//   - 1 is returned otherwise
unsigned char FSSerial2::openSerialPortWithBaud(const char* serialPortFile, speed_t baudRate)
{
	int success;
	
	// close the port if it is already open
	if (serialFileDescriptor != -1) {
		close(serialFileDescriptor);
		serialFileDescriptor = -1;
		
		// wait for the reading thread to die
		while(readThreadRunning);
		
		// re-opening the same port REALLY fast will fail spectacularly... better to sleep a sec
		sleep(0.5f);
	}
	
	// c-string path to serial-port file
	const char *bsdPath = serialPortFile;
	
	// Hold the original termios attributes we are setting
	struct termios options;
	
	// receive latency ( in microseconds )
	unsigned long mics = 3;
	
	// error message string
	int errorMessage = 0;
	
	// open the port
	//     O_NONBLOCK causes the port to open without any delay (we'll block with another call)
	serialFileDescriptor = open(bsdPath, O_RDWR | O_NOCTTY | O_NONBLOCK );
  
  printf("serialFileDescriptor=%d \n",serialFileDescriptor);
	
	if (serialFileDescriptor == -1) { 
		// check if the port opened correctly
		//errorMessage = @"Error: couldn't open serial port";
    printf("%s Error: couldn't open serial port \n",__PRETTY_FUNCTION__);
    errorMessage = 1;
	} else {
		// TIOCEXCL causes blocking of non-root processes on this serial-port
		success = ioctl(serialFileDescriptor, TIOCEXCL);
		if ( success == -1) { 
			//errorMessage = @"Error: couldn't obtain lock on serial port";
      printf("%s Error: couldn't obtain lock on serial port \n",__PRETTY_FUNCTION__);
      errorMessage = 2;
		} else {
			success = fcntl(serialFileDescriptor, F_SETFL, 0);
			if ( success == -1) { 
				// clear the O_NONBLOCK flag; all calls from here on out are blocking for non-root processes
				//errorMessage = @"Error: couldn't obtain lock on serial port";
        printf("%s Error: couldn't obtain lock on serial port \n",__PRETTY_FUNCTION__);
        errorMessage = 3;
			} else {
				// Get the current options and save them so we can restore the default settings later.
				success = tcgetattr(serialFileDescriptor, &gOriginalTTYAttrs);
				if ( success == -1) { 
					//errorMessage = @"Error: couldn't get serial attributes";
          printf("%s Error: couldn't get serial attributes \n",__PRETTY_FUNCTION__);
          errorMessage = 4;
        } else {
					// copy the old termios settings into the current
					//   you want to do this so that you get all the control characters assigned
					options = gOriginalTTYAttrs;
					
					/*
					 cfmakeraw(&options) is equivilent to:
					 options->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
					 options->c_oflag &= ~OPOST;
					 options->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
					 options->c_cflag &= ~(CSIZE | PARENB);
					 options->c_cflag |= CS8;
					 */
					cfmakeraw(&options);
					
					// set tty attributes (raw-mode in this case)
					success = tcsetattr(serialFileDescriptor, TCSANOW, &options);
					if ( success == -1) {
						//errorMessage = @"Error: coudln't set serial attributes";
            printf("%s Error: couldn't get serial attributes \n",__PRETTY_FUNCTION__);
            errorMessage = 5;
          } else {
						// Set baud rate (any arbitrary baud rate can be set this way)
						success = ioctl(serialFileDescriptor, IOSSIOSPEED, &baudRate);
						if ( success == -1) { 
							//errorMessage = @"Error: Baud Rate out of bounds";
              printf("%s Error: Baud Rate out of bounds \n",__PRETTY_FUNCTION__);
              errorMessage = 6;
						} else {
							// Set the receive latency (a.k.a. don't wait to buffer data)
							success = ioctl(serialFileDescriptor, IOSSDATALAT, &mics);
							if ( success == -1) { 
                printf("%s Error: coudln't set serial latency \n",__PRETTY_FUNCTION__);
                errorMessage = 7;
								//errorMessage = @"Error: coudln't set serial latency";
							}
						}
					}
				}
			}
		}
	}

	// make sure the port is closed if a problem happens
	if ((serialFileDescriptor != -1) && (errorMessage != 0)) {
		close(serialFileDescriptor);
		serialFileDescriptor = -1;
	}
	return errorMessage; 
}

void FSSerial2::writeByte(char byte)
{
  //printf("write char \n");
  if(serialFileDescriptor==-1){
    printf("Serial port not initialized. Make sure to select one. \n");
  }
  
  long numBytes = write(serialFileDescriptor, &byte,1);
        
	if (numBytes == -1){
    printf("Error writing to modem - %s(%d).\n", strerror(errno), errno);
  }
}

// This selector/function will be called as another thread...
//  this thread will read from the serial port and exits when the port is closed
unsigned char FSSerial2::readByte(void)
{
	const int BUFFER_SIZE = 1;
	char byte; // buffer for holding incoming data
	int numBytes=0; // number of bytes read during read
	
  if(serialFileDescriptor!=-1){
    numBytes = read(serialFileDescriptor, &byte, BUFFER_SIZE); // read up to the size of the buffer
    return byte;
  }
  return 0;
}

void FSSerial2::closeSerialPort()
{
  this->closeSerialPort(serialFileDescriptor);
}


// Given the file descriptor for a serial device, close that device.
void FSSerial2::closeSerialPort(int fileDescriptor)
{
    // Block until all written output has been sent from the device.
    // Note that this call is simply passed on to the serial device driver. 
    // See tcsendbreak(3) ("man 3 tcsendbreak") for details.
    if (tcdrain(fileDescriptor) == -1)
    {
        printf("Error waiting for drain - %s(%d).\n",
            strerror(errno), errno);
    }
    
    // Traditionally it is good practice to reset a serial port back to
    // the state in which you found it. This is why the original termios struct
    // was saved.
    if (tcsetattr(fileDescriptor, TCSANOW, &gOriginalTTYAttrs) == -1)
    {
        printf("Error resetting tty attributes - %s(%d).\n",
            strerror(errno), errno);
    }

    close(fileDescriptor);
}