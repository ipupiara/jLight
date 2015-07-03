

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

	
	typedef struct minuteBuffer
	{
		union {
			struct data {
				uint16_t  amtEntries;
				uint16_t buffer [600];
			} data;
			uint8_t  dataBytes [1];
		} ;
	} minuteBuffer;
	
	typedef minuteBuffer * pMinuteBuffer;
	
	int8_t syncStoreMinuteBuffer(pMinuteBuffer pMinB);
	
	int8_t syncRestoreMinuteBuffer(pMinuteBuffer pMinB);
	
	minuteBuffer currentMinuteBuffer;
	pMinuteBuffer pCurrentMinuteBuffer;


#endif
