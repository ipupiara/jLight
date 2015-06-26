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
	DDRA &= 0x0F;              //  PA4..PA7 as input
	PORTA = 0x70;         // set pullups on  on 3 of the buttons
	PCICR |= (1<<PCIE0);		// PA interrupt enable
	PCMSK0 |= (1<<PCINT7) | (1<< PCINT6) | (1<<PCINT5) |(1<<PCINT4) ;   // on PA4 .. PA7
	prevPinA = PINA;  
}	

enum {
		programing = 1,
		record,
		test,
		store,
	};

typedef struct   {
	int16_t port;
	int8_t  pin;
	} PortAdrRec;
	
#define portD	0x24

PortAdrRec portAdrs  [store]	=  {{portD,  PORTD0 }, {portD, PORTD1 },{portD, PORTD2 },{portD, PORTD3 }};


void setLight(PortAdrRec rAdr, int8_t direction)
{
	int16_t**  pPtr;
	pPtr = (int16_t **) rAdr.port;
	if (direction == on)  {
		**pPtr |=  (1 << rAdr.pin) ;
	} else {
		**pPtr &=  ~(1 << rAdr.pin) ;
	}
}



void setRecordLight(int8_t toState)
{
	setLight(portAdrs [record]  ,off);
}

void setTestLight(int8_t toState)
{
	setLight(portAdrs [test]  ,off);
}

void setStoreLight(int8_t toState)
{
	setLight(portAdrs [store]  ,off);
}

void setProgramingLight(int8_t toState)
{
	setLight(portAdrs [programing]  ,off);
}

ISR(PCINT0_vect )
{
	if (buttonSec10Dist == 0)  {
		if ((prevPinA & (1<< PINA4) ) != (PINA & (1<<PINA4)  ) )	{
			if (PINA & (1<<PINA4)  ) {
				storeButtonPressed = 1;
			}  else  {
			}						
		}
		if ((prevPinA & (1<< PINA5) ) != (PINA & (1<<PINA5)  ) )	{
			if (PINA & (1<<PINA5)  ) {
					recordButtonOn = 1;
				}  else  {
					recordButtonOff = 1;
			}
		}
		if ((prevPinA & (1<< PINA6) ) != (PINA & (1<<PINA6)  ) )	{
			if (PINA & (1<<PINA6)  ) {
					testButtonOn = 1;
				}  else  {
					testButtonOff = 1;
			}
		}
		if ((prevPinA & (1<< PINA7) ) != (PINA & (1<<PINA7)  ) )	{
			if (PINA & (1<<PINA7)  ) {
				programmingSwitchOn = 1;
			}  else  {
				programmingSwitchOff = 1;
			}
		} 
	}
	buttonSec10Dist = 2;
	prevPinA = PINA;
}