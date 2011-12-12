//
//  FSSerial.cpp
//  FabScan3D
//
//  Created by Francis Engelmann on 7/1/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//  Modified code from Apples serialExample
//

#include "FSSerial.h"

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

#ifdef __MWERKS__
#define __CF_USE_FRAMEWORK_INCLUDES__
#endif

#include <CoreFoundation/CoreFoundation.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#if defined(MAC_OS_X_VERSION_10_3) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3)
#include <IOKit/serial/ioss.h>
#endif
#include <IOKit/IOBSD.h>

#define LOCAL_ECHO 

#define kATCommandString	"AT\r"

#ifdef LOCAL_ECHO
#define kOKResponseString	"AT\r\r\nOK\r\n"
#else
#define kOKResponseString	"\r\nOK\r\n"
#endif

// ---------------------------------------------
#pragma mark -
#pragma mark Constrcutors and Destructors
// ---------------------------------------------

FSSerial::FSSerial(speed_t baudrate)
{
  this->bsdPath[0]=0;
  this->init(baudrate);
}

FSSerial::FSSerial(speed_t baudrate, char *bsdPath)
{
  this->bsdPath[0] = *bsdPath;
  this->init(bsdPath, baudrate);
}

//method called by the constructors so multiple constructors can be used
int FSSerial::init(speed_t baudrate){
  io_iterator_t	serialPortIterator;
  char bsdPath[50];
  bsdPath[0]=0;
    
  FSSerial::FindModems(&serialPortIterator);
  FSSerial::GetModemPath(serialPortIterator, bsdPath, sizeof(bsdPath));
    
  IOObjectRelease(serialPortIterator);	// Release the iterator.
    
  if (bsdPath[0]!=0){
    return init(bsdPath,baudrate);
  }else{
    printf("%s Couldn't find any serial port. Make sure something is connected and not used othwise.",__PRETTY_FUNCTION__);
    return -1;
  }
}

int FSSerial::init(char *bsdPath, speed_t baudrate){
  this->speed = baudrate;
  fileDescriptor = this->OpenSerialPort(bsdPath);
  if (fileDescriptor==-1){
  printf("%s unable to open specified serial port",__PRETTY_FUNCTION__);
  return -1;}
  return 1;
}

FSSerial::~FSSerial()
{
  printf("%s destructor\n",__PRETTY_FUNCTION__);
  this->CloseSerialPort(fileDescriptor);
}

// ---------------------------------------------
#pragma mark -
#pragma mark Public Writing Methods
// ---------------------------------------------

void FSSerial::writeChar(char byte){
return ;
  //printf("write char \n");
  if(fileDescriptor==-1){
    printf("Serial port not initialized. Trying to reinit. \n");
    if (this->reinit()==1) {
      this->writeChar(byte);
    }
    return;
  }
  long numBytes = write(fileDescriptor, &byte,1);
  //printf("%s writing stuff to serial port now: %ld \n",__PRETTY_FUNCTION__,numBytes);
        
	if (numBytes == -1){
    printf("Error writing to modem - %s(%d).\n", strerror(errno), errno);
    if (this->reinit()==1){
      printf("ok cool found it, should work now \n");
      this->writeChar(byte);
    }
  }
}

void FSSerial::writeString(const char* string){
  if(fileDescriptor==-1){
    printf("Serial port not initialized. Trying to reinit. \n");
    if (this->reinit()==1) this->writeString(string);
    return;
  }
  long numBytes = write(fileDescriptor, &string,strlen(string));
  if (numBytes == -1){
			printf("Error writing to modem - %s(%d).\n", strerror(errno), errno);
      if (this->reinit()==1){this->writeString(string);}
  }else{
			printf("Wrote %ld bytes \n", numBytes);
  }
}

// ---------------------------------------------
#pragma mark -
#pragma mark Public Static Methods
// ---------------------------------------------

// Returns an iterator across all known modems. Caller is responsible for
// releasing the iterator when iteration is complete.
kern_return_t FSSerial::FindModems(io_iterator_t *matchingServices)
{
    kern_return_t			kernResult; 
    CFMutableDictionaryRef	classesToMatch;

/*! @function IOServiceMatching
    @abstract Create a matching dictionary that specifies an IOService class match.
    @discussion A very common matching criteria for IOService is based on its class. IOServiceMatching will create a matching dictionary that specifies any IOService of a class, or its subclasses. The class is specified by C-string name.
    @param name The class name, as a const C-string. Class matching is successful on IOService's of this class or any subclass.
    @result The matching dictionary created, is returned on success, or zero on failure. The dictionary is commonly passed to IOServiceGetMatchingServices or IOServiceAddNotification which will consume a reference, otherwise it should be released with CFRelease by the caller. */

    // Serial devices are instances of class IOSerialBSDClient
    classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
    if (classesToMatch == NULL)
    {
        printf("IOServiceMatching returned a NULL dictionary.\n");
    }
    else {
/*!
	@function CFDictionarySetValue
	Sets the value of the key in the dictionary.
	@param theDict The dictionary to which the value is to be set. If this
		parameter is not a valid mutable CFDictionary, the behavior is
		undefined. If the dictionary is a fixed-capacity dictionary and
		it is full before this operation, and the key does not exist in
		the dictionary, the behavior is undefined.
	@param key The key of the value to set into the dictionary. If a key 
		which matches this key is already present in the dictionary, only
		the value is changed ("add if absent, replace if present"). If
		no key matches the given key, the key-value pair is added to the
		dictionary. If added, the key is retained by the dictionary,
		using the retain callback provided
		when the dictionary was created. If the key is not of the sort
		expected by the key retain callback, the behavior is undefined.
	@param value The value to add to or replace into the dictionary. The value
		is retained by the dictionary using the retain callback provided
		when the dictionary was created, and the previous value if any is
		released. If the value is not of the sort expected by the
		retain or release callbacks, the behavior is undefined.
*/
        CFDictionarySetValue(classesToMatch,
                             CFSTR(kIOSerialBSDTypeKey),
                             CFSTR(kIOSerialBSDModemType));
        
		// Each serial device object has a property with key
        // kIOSerialBSDTypeKey and a value that is one of kIOSerialBSDAllTypes,
        // kIOSerialBSDModemType, or kIOSerialBSDRS232Type. You can experiment with the
        // matching by changing the last parameter in the above call to CFDictionarySetValue.
        
        // As shipped, this sample is only interested in modems,
        // so add this property to the CFDictionary we're matching on. 
        // This will find devices that advertise themselves as modems,
        // such as built-in and USB modems. However, this match won't find serial modems.
    }
    
    /*! @function IOServiceGetMatchingServices
        @abstract Look up registered IOService objects that match a matching dictionary.
        @discussion This is the preferred method of finding IOService objects currently registered by IOKit. IOServiceAddNotification can also supply this information and install a notification of new IOServices. The matching information used in the matching dictionary may vary depending on the class of service being looked up.
        @param masterPort The master port obtained from IOMasterPort().
        @param matching A CF dictionary containing matching information, of which one reference is consumed by this function. IOKitLib can contruct matching dictionaries for common criteria with helper functions such as IOServiceMatching, IOOpenFirmwarePathMatching.
        @param existing An iterator handle is returned on success, and should be released by the caller when the iteration is finished.
        @result A kern_return_t error code. */

    kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, classesToMatch, matchingServices);    
    if (KERN_SUCCESS != kernResult)
    {
        printf("IOServiceGetMatchingServices returned %d\n", kernResult);
		goto exit;
    }
        
exit:
    return kernResult;
}

// Given an iterator across a set of modems, return the BSD path to the first one.
// If no modems are found the path name is set to an empty string.
kern_return_t FSSerial::GetModemPath(io_iterator_t serialPortIterator, char *bsdPath, CFIndex maxPathSize)
{
    io_object_t		modemService;
    kern_return_t	kernResult = KERN_FAILURE;
    Boolean			modemFound = false;
    
    // Initialize the returned path
    *bsdPath = '\0';
    
    // Iterate across all modems found. In this example, we bail after finding the first modem.
    
    while ((modemService = IOIteratorNext(serialPortIterator)) && !modemFound)
    {
        CFTypeRef	bsdPathAsCFString1;
        CFStringRef	bsdPathAsCFString2;

		// Get the callout device's path (/dev/cu.xxxxx). The callout device should almost always be
		// used: the dialin device (/dev/tty.xxxxx) would be used when monitoring a serial port for
		// incoming calls, e.g. a fax listener.
	
		bsdPathAsCFString1 = IORegistryEntryCreateCFProperty(modemService,
                                                            CFSTR(kIOCalloutDeviceKey),
                                                            kCFAllocatorDefault,
                                                            0);
        if (bsdPathAsCFString1)
        {
            Boolean result;
            
            // Convert the path from a CFString to a C (NUL-terminated) string for use
			// with the POSIX open() call.
	    
      bsdPathAsCFString2 = (CFStringRef)bsdPathAsCFString1;
      
			result = CFStringGetCString(bsdPathAsCFString2,
                                        bsdPath,
                                        maxPathSize, 
                                        kCFStringEncodingUTF8);
            CFRelease(bsdPathAsCFString1);
            
            if (result)
			{
                printf("Modem found with BSD path: %s", bsdPath);
                modemFound = true;
                kernResult = KERN_SUCCESS;
            }
        }

        printf("\n");

        // Release the io_service_t now that we are done with it.
	
		(void) IOObjectRelease(modemService);
    }
        
    return kernResult;
}

// ---------------------------------------------
#pragma mark -
#pragma mark Private Open/Close Methods
// ---------------------------------------------

/**
 * Use this method to try to reopen the serial port
 * @return 0 an failure, 1 on success
 */
int FSSerial::reinit(void)
{
  //Make sure Port is really closed when doing this
  /*
  this->CloseSerialPort(fileDescriptor);
  
  if(bsdPath[0]!=0){ //bsdPath has been specified so use it
    printf("Reinit with specified bsdPath %s \n",bsdPath);
    if(!init(bsdPath,speed)){
      bsdPath[0]=0; //clear specified path is it is not working
      return -1;
    }
  }else{
    printf("Init first free serial port \n");
    if(init(speed)==-1) return -1;
  }
  */
  return -1;
}

// Given the path to a serial device, open the device and configure it.
// Return the file descriptor associated with the device.
int FSSerial::OpenSerialPort(const char *bsdPath)
{
    printf("%s \n",__PRETTY_FUNCTION__);


    int				fileDescriptor = -1;
    int				handshake;
    struct termios	options;
    
    // Open the serial port read/write, with no controlling terminal, and don't wait for a connection.
    // The O_NONBLOCK flag also causes subsequent I/O on the device to be non-blocking.
    // See open(2) ("man 2 open") for details.
    
    fileDescriptor = open(bsdPath, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fileDescriptor == -1)
    {
        printf("Error opening serial port %s - %s(%d).\n",
               bsdPath, strerror(errno), errno);
        if (fileDescriptor != -1){ close(fileDescriptor); } return -1;
    }

    // Note that open() follows POSIX semantics: multiple open() calls to the same file will succeed
    // unless the TIOCEXCL ioctl is issued. This will prevent additional opens except by root-owned
    // processes.
    // See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
    
    if (ioctl(fileDescriptor, TIOCEXCL) == -1)
    {
        printf("Error setting TIOCEXCL on %s - %s(%d).\n",
            bsdPath, strerror(errno), errno);
        if (fileDescriptor != -1){ close(fileDescriptor); } return -1;
    }
    
    // Now that the device is open, clear the O_NONBLOCK flag so subsequent I/O will block.
    // See fcntl(2) ("man 2 fcntl") for details.
    
    if (fcntl(fileDescriptor, F_SETFL, 0) == -1)
    {
        printf("Error clearing O_NONBLOCK %s - %s(%d).\n",
            bsdPath, strerror(errno), errno);
        if (fileDescriptor != -1){ close(fileDescriptor); } return -1;
    }
    
    // Get the current options and save them so we can restore the default settings later.
    if (tcgetattr(fileDescriptor, &gOriginalTTYAttrs) == -1)
    {
        printf("Error getting tty attributes %s - %s(%d).\n",
            bsdPath, strerror(errno), errno);
        if (fileDescriptor != -1){ close(fileDescriptor); } return -1;
    }

    // The serial port attributes such as timeouts and baud rate are set by modifying the termios
    // structure and then calling tcsetattr() to cause the changes to take effect. Note that the
    // changes will not become effective without the tcsetattr() call.
    // See tcsetattr(4) ("man 4 tcsetattr") for details.
    
    options = gOriginalTTYAttrs;
    
    // Print the current input and output baud rates.
    // See tcsetattr(4) ("man 4 tcsetattr") for details.
    
    printf("Current input baud rate is %d\n", (int) cfgetispeed(&options));
    printf("Current output baud rate is %d\n", (int) cfgetospeed(&options));
    
    // Set raw input (non-canonical) mode, with reads blocking until either a single character 
    // has been received or a one second timeout expires.
    // See tcsetattr(4) ("man 4 tcsetattr") and termios(4) ("man 4 termios") for details.
    
    cfmakeraw(&options);
    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 10;
        
    // The baud rate, word length, and handshake options can be set as follows:
    
    cfsetspeed(&options, speed);		// Set 9200 baud    
    //cfsetspeed(&options, 9600);		// Set 9200 baud    
    options.c_cflag |= (CS7 	   | 	// Use 7 bit words
						PARENB	   | 	// Parity enable (even parity if PARODD not also set)
						CCTS_OFLOW | 	// CTS flow control of output
						CRTS_IFLOW);	// RTS flow control of input
			
//#if defined(MAC_OS_X_VERSION_10_4) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4)
	// Starting with Tiger, the IOSSIOSPEED ioctl can be used to set arbitrary baud rates
	// other than those specified by POSIX. The driver for the underlying serial hardware
	// ultimately determines which baud rates can be used. This ioctl sets both the input
	// and output speed. 
	
  //defined in header
	//speed_t speed = 9600; // Set 14400 baud
    if (ioctl(fileDescriptor, IOSSIOSPEED, &speed) == -1)
    {
        printf("Error calling ioctl(..., IOSSIOSPEED, ...) %s - %s(%d).\n",
            bsdPath, strerror(errno), errno);
    }
//#endif
    
    // Print the new input and output baud rates. Note that the IOSSIOSPEED ioctl interacts with the serial driver 
	// directly bypassing the termios struct. This means that the following two calls will not be able to read
	// the current baud rate if the IOSSIOSPEED ioctl was used but will instead return the speed set by the last call
	// to cfsetspeed.
    
    printf("Input baud rate changed to %d\n", (int) cfgetispeed(&options));
    printf("Output baud rate changed to %d\n", (int) cfgetospeed(&options));
    
    // Cause the new options to take effect immediately.
    if (tcsetattr(fileDescriptor, TCSANOW, &options) == -1)
    {
        printf("Error setting tty attributes %s - %s(%d).\n",
            bsdPath, strerror(errno), errno);
        if (fileDescriptor != -1){ close(fileDescriptor); } return -1;
    }

    // To set the modem handshake lines, use the following ioctls.
    // See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
    
    if (ioctl(fileDescriptor, TIOCSDTR) == -1) // Assert Data Terminal Ready (DTR)
    {
        printf("Error asserting DTR %s - %s(%d).\n",
            bsdPath, strerror(errno), errno);
    }
    
    if (ioctl(fileDescriptor, TIOCCDTR) == -1) // Clear Data Terminal Ready (DTR)
    {
        printf("Error clearing DTR %s - %s(%d).\n",
            bsdPath, strerror(errno), errno);
    }
    
    handshake = TIOCM_DTR | TIOCM_RTS | TIOCM_CTS | TIOCM_DSR;
    if (ioctl(fileDescriptor, TIOCMSET, &handshake) == -1)
    // Set the modem lines depending on the bits set in handshake
    {
        printf("Error setting handshake lines %s - %s(%d).\n",
            bsdPath, strerror(errno), errno);
    }
    
    // To read the state of the modem lines, use the following ioctl.
    // See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
    
    if (ioctl(fileDescriptor, TIOCMGET, &handshake) == -1)
    // Store the state of the modem lines in handshake
    {
        printf("Error getting handshake lines %s - %s(%d).\n",
            bsdPath, strerror(errno), errno);
    }
    
    printf("Handshake lines currently set to %d\n", handshake);
	
#if defined(MAC_OS_X_VERSION_10_3) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3)
	unsigned long mics = 1UL;

	// Set the receive latency in microseconds. Serial drivers use this value to determine how often to
	// dequeue characters received by the hardware. Most applications don't need to set this value: if an
	// app reads lines of characters, the app can't do anything until the line termination character has been
	// received anyway. The most common applications which are sensitive to read latency are MIDI and IrDA
	// applications.
	
	if (ioctl(fileDescriptor, IOSSDATALAT, &mics) == -1)
	{
		// set latency to 1 microsecond
        printf("Error setting read latency %s - %s(%d).\n",
            bsdPath, strerror(errno), errno);
        goto error;
	}
#endif
    
    // Success
    return fileDescriptor;
    
    // Failure path
error:
    if (fileDescriptor != -1)
    {
        close(fileDescriptor);
    }
    
    return -1;
}

// Replace non-printable characters in str with '\'-escaped equivalents.
// This function is used for convenient logging of data traffic.
char* FSSerial::LogString(char *str)
{
    static char     buf[2048];
    char            *ptr = buf;
    int             i;

    *ptr = '\0';

    while (*str)
	{
		if (isprint(*str))
		{
			*ptr++ = *str++;
		}
		else {
			switch(*str)
			{
				case ' ':
					*ptr++ = *str;
					break;

				case 27:
					*ptr++ = '\\';
					*ptr++ = 'e';
					break;

				case '\t':
					*ptr++ = '\\';
					*ptr++ = 't';
					break;

				case '\n':
					*ptr++ = '\\';
					*ptr++ = 'n';
					break;

				case '\r':
					*ptr++ = '\\';
					*ptr++ = 'r';
					break;

				default:
					i = *str;
					(void)sprintf(ptr, "\\%03o", i);
					ptr += 4;
					break;
			}

			str++;
		}

		*ptr = '\0';
	}

    return buf;
}

// Given the file descriptor for a modem device, attempt to initialize the modem by sending it
// a standard AT command and reading the response. If successful, the modem's response will be "OK".
// Return true if successful, otherwise false.
Boolean FSSerial::InitializeModem(int fileDescriptor)
{
    char		buffer[256];	// Input buffer
    char		*bufPtr;		// Current char in buffer
    ssize_t		numBytes;		// Number of bytes read or written
    int			tries;			// Number of tries so far
    Boolean		result = false;
    
    for (tries = 1; tries <= 3; tries++)
    {
        printf("Try #%d\n", tries);
        
        // Send an AT command to the modem
        numBytes = write(fileDescriptor, kATCommandString, strlen(kATCommandString));
        
		if (numBytes == -1)
		{
			printf("Error writing to modem - %s(%d).\n", strerror(errno), errno);
			continue;
		}
		else {
			printf("Wrote %ld bytes \"%s\"\n", numBytes, LogString(kATCommandString));
		}
	
		if (numBytes < strlen(kATCommandString))
		{
            continue;
		}
	
        printf("Looking for \"%s\"\n", LogString(kOKResponseString));
	
		// Read characters into our buffer until we get a CR or LF
        bufPtr = buffer;
        do
        {
            numBytes = read(fileDescriptor, bufPtr, &buffer[sizeof(buffer)] - bufPtr - 1);
            if (numBytes == -1)
            {
                printf("Error reading from modem - %s(%d).\n", strerror(errno), errno);
            }
            else if (numBytes > 0)
            {
                bufPtr += numBytes;
                if (*(bufPtr - 1) == '\n' || *(bufPtr - 1) == '\r')
				{
                    break;
				}
            }
            else {
                printf("Nothing read.\n");
            }
        } while (numBytes > 0);
        
        // NUL terminate the string and see if we got an OK response
        *bufPtr = '\0';
        
        printf("Read \"%s\"\n", LogString(buffer));
        
        if (strncmp(buffer, kOKResponseString, strlen(kOKResponseString)) == 0)
        {
            result = true;
            break;
        }
    }

    return result;
}

// Given the file descriptor for a serial device, close that device.
void FSSerial::CloseSerialPort(int fileDescriptor)
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
