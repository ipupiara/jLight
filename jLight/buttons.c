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


uint8_t currentPinA;


void initButtons()
{
	DDRA &= 0x0F;              //  PA4..PA7 as input
	PCICR |= (1<<PCIE0);		// PA interrupt enable
	PCMSK0 |= (1<<PCINT7) | (1<< PCINT6) | (1<<PCINT5) |(1<<PCINT4) ;   // on PA4 .. PA7
	currentPinA = PINA;
} 


ISR(PCINT0_vect )
{
	if (buttonSec10Dist == 0)  {
		if ((currentPinA & (1<< PINA4) ) != (PINA & (1<<PINA4)  ) )	{
			if (PINA & (1<<PINA4)  ) {
				programmingSwitchOn = 1;
			}  else  {
				programmingSwitchOff = 1;
			}
		}
		if ((currentPinA & (1<< PINA5) ) != (PINA & (1<<PINA5)  ) )	{
			if (PINA & (1<<PINA5)  ) {
				recordButtonOn = 1;
				}  else  {
				recordButtonOff = 1;
			}
		}
		if ((currentPinA & (1<< PINA6) ) != (PINA & (1<<PINA6)  ) )	{
			if (PINA & (1<<PINA6)  ) {
				testButtonOn = 1;
				}  else  {
				testButtonOff = 1;
			}
		}
		if ((currentPinA & (1<< PINA7) ) != (PINA & (1<<PINA7)  ) )	{
			if (PINA & (1<<PINA7)  ) {
				storeButtonPressed = 1;
				}  else  {
		
			}
		}
		buttonSec10Dist = 2;
	}
	currentPinA = PINA;
}