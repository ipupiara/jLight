/*
 * buttons.c
 *
 * Created: 6/13/2015 4:58:11 PM
 *  Author: ixchel
 */ 


#include <avr/io.h>
#include <stdio.h>

#include "buttons.h"

#include "TriacDefines.h"
#include "TriacIntr.h"
// #include "triacPID.h"


uint8_t prevPinA;



void initButtons()
{
	DDRA &= 0x0E;              // PA0, PA4..PA7 as input 
	PORTA = 0x70;         // set pullups on  on 3 of the buttons, rest to 0
	PCICR |= (1<<PCIE0);		// PA interrupt enable
	PCMSK0 |= (1<<PCINT7) | (1<< PCINT6) | (1<<PCINT5) |(1<<PCINT4) ;   // on PA4 .. PA7
	
	DDRA |= 0x0E;     // PA1 .. PA3   as output
	DDRD |= (1<< DDD7 );     //  PD7  as output
	PORTD  &= ~(1<<PD7);  // PD7 set to 0
	prevPinA = PINA;  
}	

enum {
		programing = 0,
		record,
		test,
		store,
		numLights
	};

typedef struct   {
	int16_t port;
	int8_t  pin;
	} PortAdrRec;
	
#define portD	0x2B
#define portA   0x22

PortAdrRec portAdrs  [numLights]	=  {{portD,  PORTD7 }, {portA, PORTA1 },{portA, PORTA2 },{portA, PORTA3 }};


void setLight(PortAdrRec rAdr, int8_t direction)
{
	uint8_t*   vPtr;
	vPtr  = (uint8_t *) rAdr.port;
//	*pPtr = (int16_t *) rAdr.port;
	if (direction == on)  {
		*vPtr |=  (1 << rAdr.pin) ;
	} else {
		* vPtr &=  ~(1 << rAdr.pin) ;
	}
}



void setRecordLight(int8_t toState)
{
	setLight(portAdrs [record]  ,toState);
}

void setTestLight(int8_t toState)
{
	setLight(portAdrs [test]  ,toState);
}

void setStoreLight(int8_t toState)
{
	setLight(portAdrs [store]  ,toState);
}

void setProgramingLight(int8_t toState)
{
	setLight(portAdrs [programing]  ,toState);
}

ISR(PCINT0_vect )
{
	cli();
//	if (buttonSec10Dist == 0)  {
		if ((prevPinA & (1<< PINA4) ) != (PINA & (1<<PINA4)  ) )	{
			if (PINA & (1<<PINA4)  ) {
				storeButtonPressed = 0;
			}  else  {
				storeButtonPressed = 1;
			}						
		}
		if ((prevPinA & (1<< PINA5) ) != (PINA & (1<<PINA5)  ) )	{
			if (PINA & (1<<PINA5)  ) {
					recordButtonOff = 1;
				}  else  {
					recordButtonOn = 1;
			}
		}
		if ((prevPinA & (1<< PINA6) ) != (PINA & (1<<PINA6)  ) )	{
			if (PINA & (1<<PINA6)  ) {
					testButtonOff = 1;
				}  else  {
					testButtonOn = 1;
			}
		}
		if ((prevPinA & (1<< PINA7) ) != (PINA & (1<<PINA7)  ) )	{
			if (PINA & (1<<PINA7)  ) {
				programmingSwitchOn = 1;
			}  else  {
				programmingSwitchOff = 1;
			}
		} 
	
//}
//	buttonSec10Dist = 2;
	prevPinA = PINA;
	sei();
}


void checkProgramingButton()
{
	if (PINA & (1<<PINA7)  ) {
		programmingSwitchOn = 1;
		printf("checkButtons found programingButton on\n");
	}
			

}
