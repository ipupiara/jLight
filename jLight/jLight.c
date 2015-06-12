/*
 * jLight.c
 *
 * Created: 6/4/2015 3:11:24 PM
 *  Author: ixchel
 */ 


#define F_CPU 11095200

#include <avr/io.h>
#include <stdio.h>
#include <string.h>

#include "StateClass.h"
#include "TriacIntr.h"



void USART_Init( unsigned int baud )
{
	// Set baud rate
	
	UBRR0H = (unsigned char)(baud>>8);
	UBRR0L = (unsigned char)baud;
	// Enable receiver and transmitter
	UCSR0A =  (UCSR0A & 0b11111100) ;
	
	UCSR0B=(1<<RXEN0)|(1<<TXEN0);
	//	UCSR0B = 0b00011000;  // rx compl intr ena - tx compl intr ena - dreg empty intr ena - rx ena - tx ena - sz2 (size bit 2)  - 9. bit rx - 9. tx

	UCSR0C = 0b00000110; // "00" async usart - "00" disa parity - 1 (or 2) stop bit - sz1 - sz0 (set t0 8 bit) - clk polarity (sync only)
}


static int uart_putchar(char c, FILE *stream);

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);  // first opened will be stdout and errout

//stdout = fdevopen((FILE *)uart_putchar, NULL, 0);

static int uart_putchar(char c, FILE *stream)
{
	while (!(UCSR0A & (1<<UDRE0)));
	if (c == '\n')
	{
		uart_putchar('\r',&mystdout);
	}
	UDR0 = c;
	return 0;
}




int main(void)
{
	CJLightEvent ev;
	
//	USART_Init( 143 );   // baud 4800 at 11.0592 mhz, single uart speed
//	USART_Init( 71 );   // baud 9600 at 11.0592 mhz, single uart speed
	USART_Init (11 );   // baud 57.6k  at 11.0592 mhz, single uart speed
	stdout = &mystdout;
	printf("\nSTARTUP\n");
	restorePersistentData();

	initInterrupts();

	startStateCharts();	

/*	int dummyI;
	dummyI = 0;
	while(1) {
		++ dummyI;
		if (dummyI == 0) {
			if ((ky = keyEntered())){
					printf("\nmain() ky %c %X",ky,ky);
			}
		}
*/	
	while (1)
	{
//		checkDebugBuffer();

		if (fatalErrorOccurred) {     // do this with highest priority (at the beginning)
//			fatalErrorOccurred = 0;	  // once occurred state stays until restart/reset
			ev.evType = evFatalError;
			processTriacEvent(&SJLightTriacStateChart,&ev);	
		}
/*		if (twiDataReceived ==  1) {
			twiDataReceived = 0;
			ev.evType = evTWIDataReceived;
			processTriacEvent(&SJoesTriacStateChart,&ev);	
		}
		if (twiDataSent ==  1) {
			twiDataSent = 0;
		}
*/
		if (adcTick){
			adcTick = 0; // 8-bit access is atomic
			ev.evType = evAdcTick;
			processTriacEvent(&SJLightTriacStateChart  ,&ev);	
		}
		if (runningSecondsTick){
	//		cli();    // 8-bit access is already atomic
			runningSecondsTick = 0;
	//		sei();
			ev.evType = evSecondsTick;
			processTriacEvent(&SJLightTriacStateChart,&ev);	
		}

		if (durationTimerReachead) {
	//		cli();   // 8-bit access is alread atomic
			durationTimerReachead = 0;
	//		sei();
			ev.evType = evTimeOutDurationTimer;
			processTriacEvent(&SJLightTriacStateChart,&ev);	
		}

		if (sec10Tick) {
			//		cli();   // 8-bit access is alread atomic
			sec10Tick = 0;
			//		sei();
			ev.evType = evSec10Tick;
			processTriacEvent(&SJLightTriacStateChart,&ev);
		}


	/*	if ((ky = keyEntered())){
			printf("ky %c %X\n",ky,ky);
			if (ky == kpF1) ev.evType = evF1Pressed;
			if (ky == kpF2) ev.evType = evF2Pressed;   
			if (ky == kpStart) ev.evType = evStartPressed;
			if (ky == kpStop) ev.evType = evStopPressed;
			if (ky == kpAst) ev.evType = evAstPressed;
			if (ky == kpNum) ev.evType = evNumPressed;
			if (ky >= kp0){
				ev.evType = evCharEntered;
				ev.evData.keyCode = ky;			
			}
//			printf("ev fired key : %x, startEv:%i, stopEv:%i\n",ky,(ev.evType==evStartPressed),(ev.evType==evStopPressed));	
			processTriacEvent(&SJoesTriacStateChart,&ev);	
		}
		if (stableZeroAdjReached) {
			stableZeroAdjReached = 0;
			ev.evType = evZeroSignalOK;
			processTriacEvent(&SJoesTriacStateChart,&ev);	
		}
	
*/
/* 		
	if (debugEvent1Triggered) {
			debugEvent1Triggered = 0;
//			ev.evType = evStartPressed;
//			processTriacEvent(&SJoesTriacStateChart,&ev);
			ev.evType = evAstPressed;
			processTriacEvent(&SJoesTriacStateChart,&ev);
		}  
		
    }*/



	}



}