//
//  FSSerial.h
//  FabScan3D
//
//  Created by Francis Engelmann on 7/1/11.
//  Copyright 2011 Media Computing Group, RWTH Aachen University. All rights reserved.
//

#ifndef FS_SERIAL_H
#define FS_SERIAL_H

#include <termios.h>
#include <IOKit/IOKitLib.h>

class FSSerial {
  private:
    int fileDescriptor;
    char bsdPath[50];  //bsdPath  - only specified when a specific serial port is addressed, otherwise chosen randomly
    speed_t speed;  //baudrate - must be specified whenever a constructor is called

    // Hold the original termios attributes so we can reset them
    struct termios gOriginalTTYAttrs;

    int init(speed_t baudrate);
    int init(char *bsdPath, speed_t baudrate);
    int reinit();
    
    // Function prototypes    
    int OpenSerialPort(const char *bsdPath);
    char *LogString(char *str);
    Boolean InitializeModem(int fileDescriptor);
    void CloseSerialPort(int fileDescriptor);    

  public:
    /* Constructors and Destructor */
    
    /**
     * Opens the first serial port found
     * @param baudrate
     */
    FSSerial(speed_t baudrate);
    
    /**
     * Opens the serial port specified by bsdPath
     * @param baudrate
     */
    FSSerial(speed_t baudrate, char *bsdPath);
    
    /**
     * Destructor
     */
    ~FSSerial();
    
    static kern_return_t FindModems(io_iterator_t *matchingServices);
    static kern_return_t GetModemPath(io_iterator_t serialPortIterator, char *bsdPath, CFIndex maxPathSize);
    
    void writeChar(char byte);
    void writeString(const char* string);
};

#endif