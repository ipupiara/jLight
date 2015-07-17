

// TriacDefines.h contains global and some local defines, but not all of the local defines
// also it contains many of the global data definition and their persistent storage on eeprom

// Yoar Attention please, yoar attention: set Brown out level on 2.7V at least
// otherwise errors on the eeprom could be observed at startup

#ifndef TriacDefines_H
	#define TriacDefines_H


#include <inttypes.h>

//	#define jtagDebugKeyboardMode	// tobe set if jtag debug is used, it will divert the serial input (terminal)
									//  to the keyboard and leave the keyboard port (port C) free for jtag

	#define maxSecsPossible  0x7FFF


	#define JT_F_CPU 11095200	
	
//	#define calibLowCmsEEPROMpos   0
//	#define calibHighCmsEEPROMpos  2   // both int16
//	#define amps100EEPROMpos    4   // 8-bit
//	#define amps10EEPROMpos    5   // 8-bit



	#define calibLowAmps        15
	#define calibHighAmps       60

	#define triggerDelayMax 760     // works properly with 50 Hz Ac and 11.0592 hz
	
	#define inductiveLoad   0      // to be set to 0 or 1
	#define triacTriggerLength   0      // delay approx ( n * 2.5 us ) + 5.5    at 11.0592  mhz 
	#define delayBetweenTriacTriggers  16  //  in timer 2 tcnt values , prescaler at   128

// takes approx  2 us at 10 Mhz with -o0 (no opts. !) ,with all  call/ret, push N Pop
void delayEmptyProc (); 


void delay6pnt2d5us(unsigned int n);   // description in TriacDefines.c


// for simplicity reasons some of the "Database logic" and "Model logic" are implemented on the 
// Defines component. If ever the application should come to a certain complexity, refactoring on this should
// be done for clearer overview, but so far the current simplification seems acceptable


int8_t fatalErrorOccurred;

int8_t eepromAccessErrorOcurred;

void restorePersistentData();

#define zeroAdjustATtinyID  0x10


#define endGapSize         10
#define storageBufferSize  1800
#define bufferLowSize       500
#define bufferCriticalSize  250
#define bufferTimeoutSize   100
	
	typedef struct StorageBuffer
	{
		union {
			struct data {
				uint16_t  amtEntries;
				uint8_t   buffer [storageBufferSize];  
				uint8_t   endByte;
			} data;
			uint8_t  dataBytes [1];
		} ;
	} StorageBuffer;
	
	typedef StorageBuffer * PStorageBuffer;
	
	int8_t syncStoreStorageBuffer(PStorageBuffer pStoB);
	
	int8_t syncRestoreStorageBuffer(PStorageBuffer pStoB);
	
	StorageBuffer currentStorageBuffer;
	PStorageBuffer pCurrentStorageBuffer;
	
#endif
