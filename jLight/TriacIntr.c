
#include <avr/io.h>
#include <stdio.h>

//#include "uart.h"


#include "TriacIntr.h"
#include "TriacDefines.h"
// #include "triacPID.h"

//#define triggerDelayMax 810     // works properly with 50 Hz Ac


int16_t lastAmpsADCVal;

int16_t remainingTriacTriggerDelayCounts;

int16_t triacTriggerDelayTime;

int16_t secondsDurationTimerRemaining;

int16_t secondsInDurationTimer;

uint8_t adcCnt;

uint8_t int0StartCnt;




int16_t getSecondsDurationTimerRemaining()
{
	int16_t res;
	cli();
	res = secondsDurationTimerRemaining;
	sei();
	return res;
}

int16_t getSecondsInDurationTimer()
{
	int16_t res;
	cli();
	res = secondsInDurationTimer;
	sei();
	return res;
}

#define ocra2aValueMax 0XFC  // still to be defined

void setTcnt2AndOcra2a(int16_t newTcnt2Val,int16_t newOcra2a)
{
	// timer must be stopped to set tcnt, because else, on an 
	// unprotected set, the timer itself could interfere with the *non double buffered feature" write access.
	// resulting in a more or less random set value.
	int8_t tccr2bStack;
	tccr2bStack = TCCR2B;
	TCCR2B = 0b00000000  ;  // CTC, timer stopped		
	if (TCNT2 != newTcnt2Val) {  // dont set if not needed , because  .....
		TCNT2 = newTcnt2Val;	
		if (newOcra2a == (TCNT2 + 1)) {++ newOcra2a; }  // .... updating avoids triggering of next clock cycle, but needs overnext.
	}
	OCR2A = newOcra2a;  
	TCCR2B = tccr2bStack  ; // set previous value
}

void setTriacTriggerDelayValues()
{
	if (remainingTriacTriggerDelayCounts < ocra2aValueMax) {		
		setTcnt2AndOcra2a (0, remainingTriacTriggerDelayCounts);
		triacTriggerDelayTime = triacTriggerDelayTime + remainingTriacTriggerDelayCounts;
		remainingTriacTriggerDelayCounts = 0;
	} else {
		remainingTriacTriggerDelayCounts -= ocra2aValueMax;
		setTcnt2AndOcra2a(0, ocra2aValueMax);
		triacTriggerDelayTime = triacTriggerDelayTime + ocra2aValueMax;
	}
}

void startTimer2()
{
	TIFR2 = 0x00;
	TIMSK2   = 0b00000010;  //  Output Compare A Match Interrupt Enable
	TCCR2B = 0b00000101  ; // CTC on CC2A , set clk / 128, timer 2 started
}

void stopTimer2()
{
	TCCR2B = 0b00000000  ;  // CTC, timer stopped
	TIMSK2  = 0x00;
	TIFR2 = (1<< OCF2A);    // cleared by writing a "**@@!! logic ??@@**" one to the flag
}


void startTriacTriggerDelay( int16_t delayDuration)  // must run protected between cli and sei
{
	if (delayDuration <= 0) { 
		delayDuration = 1;   // just a very short duration, but one that will happen in future
	}
	remainingTriacTriggerDelayCounts = delayDuration;
	setTriacTriggerDelayValues();
	startTimer2();		
}

void immediateStopTriacTriggerDelay()   // must run protected between cli and sei
{
	cli();
	PORTD &= ~0x10;	
	stopTimer2();
	sei();
}	  

void setTriacFireDuration(uint16_t cmsecs)
{
	cli();
	if (cmsecs < triggerDelayMax) {
		if (cmsecs > 0) {
			triacFireDurationCms = cmsecs;}
		else {
			triacFireDurationCms = 0;
		}
	} else {
		triacFireDurationCms = triggerDelayMax;
	}
	sei();
}

uint16_t getTriacFireDuration()
{
	uint16_t result;
	cli();
		result = triacFireDurationCms;
	sei();
	return result;	
}

/*

		tobe tested
		just leave triac on for the "rest of the time"

*/

ISR(TIMER2_COMPA_vect)
{
	cli();
	if (remainingTriacTriggerDelayCounts <= 0) {
		PORTD |= 0x10;	
		delay6pnt2d5us(triacTriggerLength);   // approx 5 us of triac trigger , try later half or even less  , measured 7 with oscilloscope
		PORTD &= ~0x10;			// handled synchronous
		if ((triacTriggerDelayTime >= triggerDelayMax) || (! inductiveLoad)  ) {
			stopTimer2();
		} else {
			startTriacTriggerDelay(delayBetweenTriacTriggers);
		}
	} else {
		setTriacTriggerDelayValues();
	}	
	sei();
}


#define pidStepDelays  2    // 20 reads per second

ISR(ADC_vect)

{
	lastAmpsADCVal = ADC;
	
	++ adcCnt;

	if (adcCnt >= pidStepDelays)  {     
		adcCnt = 0;
		adcTick = 1;
	}
}

ISR(INT0_vect)
{
	cli();
	if ((PIND & 0x04) == 0) {
		immediateStopTriacTriggerDelay();			
	} else {
		triacTriggerDelayTime = 0;
		if (triacFireDurationCms > 0)  {
			startTriacTriggerDelay(  triggerDelayMax - triacFireDurationCms);
		}
		++int0StartCnt;
		if ((int0StartCnt % 5) == 0 ) {
			sec5Tick = 1;
			if (int0StartCnt >= 100) {
				int0StartCnt = 0;
			}
			++sec5Cnt;
			if (sec5Cnt > 9) {
				sec5Cnt = 0;
			}
			if(buttonSec10Dist >0 ) {
				--buttonSec10Dist;
			}
		}
	}
	sei();		  
}   

ISR(TIMER0_COMPA_vect)
{    // needed for ADC so far..
//	cli();
	//  	ADCSRA  |= (1<<ADSC);   // start one ADC cycle
//	sei();
}

ISR(TIMER1_COMPA_vect)
{
	secondsDurationTimerRemaining --;
	secondsInDurationTimer ++;
	if (secondsDurationTimerRemaining <= 0) {
		stopDurationTimer();
		durationTimerReachead = 1;
	} else {
		runningSecondsTick = 1;
	}
}

void initInterrupts()
{
	printf("init interrupts\n");
	cli();
// Ext. Interrupt

//		DDRA = 0b11110000;    // set pin 7 to 4 of port A as output for digital poti (zero adj)
//		PORTA = 0b11100000;
//		DIDR0 = 0x0F;			// disa digital input on a0..a3

		DIDR0 = (1<< ADC0D)  ;    // disa digital input on a0

		DDRD &= ~(1<< DDD2 ) ;		// set PortD pin 2 as input for trigger Ext Int 0
		PORTD &=  ~(1 << PD2 ) ;   // without pullup 

		PORTD &= ~0x10; 		// done also before setting DDR to avoid eventual accidental triac trigger
		DDRD |= 0x10;			// set Portd pin 04 be Triac output
		PORTD &= ~0x10; 		// and initialize with 0-value

		PORTD &= ~0x08; 		
		DDRD |= 0x08;			// set Portd pin 03 to be completionAlarm
		PORTD &= ~0x08; 		// and initialize with 0-value



		EICRA = 0x01;   // both, fall/rise edge trigger    
		EIMSK = 0x00;   

// Timer 1 as Duration Timer
	  
			runningSecondsTick = 0;
	  
		TCCR1A = 0x00;  // normal mode or CTC dep.on TCCR1B
		//TCCR1B = 0b00001101  ; // CTC on CC1A , set clk / 1024, timer started

		TCCR1B = 0b00001000  ;  // CTC, timer stopped

		TCCR1C = 0x00; // no Force output compare

		OCR1A = 0x2A30;  // counter top value  , this value at clk/1024 will cause a delay of exact 1 sec
		TCNT1 = 0x00 ;  

		TIMSK1  = 0x00; // disa  Interrupt 
		//		TIMSK1   = 0b00000010;  //  Output Compare A Match Interrupt Enable 

// Timer 0    used for ADC triggering  in TriacRunning mode
	  
		TCCR0A = 0b00000010;  //  CTC 

		OCR0A = 0xFF;  // counter top value, 0xFF means approx 42.18 ADC measures and write to mem per sec
					// far too much for our needs, but runs parallel except 
					//  the very short ADC-complete interrrupt
		TCNT0 = 0x00 ;  
  
//		TCCR0B = 0b00000101  ; // CTC on CC0A , set clk / 1024, timer started	  
//		TIMSK0  = 0b00000010;  // ena  interrupts, and let run ADC
// 		not yet start Timer0 and ADC, to be tested
		TCCR0B = 0b00000000  ; // CTC on CC0A , not yet started	  
		TIMSK0  = 0b00000000;


// Timer 2 as Triac Trigger Delay Timer
	  
		TCCR2A = 0b00000010;  //  CTC 

		//TCCR2B = 0b00000101  ; // CTC on CC0A , set clk / 128, timer started

		TCCR2B = 0b00000000  ;  // CTC, timer stopped
		ASSR = 0x00;

		OCR2A = ocra2aValueMax;  // counter top value  , just anything for start, will later be set by PID
		TCNT2 = 0x00 ;  

		TIMSK2  = 0x00; // disa  Interrupt 
		//		TIMSK2   = 0b00000010;  //  Output Compare A Match Interrupt Enable 

//  init ADC

		ADCSRA  = 0b00000111;  // disa ADC, ADATE, ADIE	
		adcTick = 0;
		adcCnt = 0;
		lastAmpsADCVal = 0;
		
		sec5Cnt = 0;
		sec5Tick = 0;
		buttonSec10Dist = 0;

		sei();  // start interrupts if not yet started
		
}


void startAmpsADC()
{
	printf("start AMPS adc\n");
	cli();
	ADCSRA  = 0b00000111;  // disa ADC, ADATE, ADIE	
	adcTick = 0;
	adcCnt = 0;

		DIDR0 = (1<< ADC0D)  ;

//	ADCSRA  = (1<<ADEN) | (1<<ADIE) | (1<<ADPS1) | (1<<ADPS2) | (1<<ADPS0); //  now 64 prescaler wihout ps0 , 128 with ps0

	ADCSRA = 0b10101111;  
							// int ena, prescale /128
							// ADC clock will run at 86400 hz, or max 6646. 
							//  read per sec,what is ok
							// for our settings of 42. read per sec	
							// or manuals start 

	ADCSRB = 0x03;  // no ACME, trigger ADC on Timer0 compare match
//	ADCSRB = 0x00 ;

	
	ADMUX = 0b01000000;


	OCR0A = 0xFF; 
	TCCR0A = (1<< WGM01);   // CTC on OCR0A
	TCCR0B = 0b00000101  ; // CTC on CC0A , set clk / 1024, timer 0 started	  
	TIMSK0  = 0b00000010;  // ena  interrupts, and let run ADC	
	sei();
}

void startTriacRun()
{
//	startAmpsADC();
	int0StartCnt = 0;
	cli();
	EIFR = 0x00;
	EIMSK = 0x01;  		
	sei();		// start external interrupt (zero pass detection)
	
	printf("Triacrun started\n");
}

void stopTriacRun()
{
	cli();
	EIMSK = 0x00;				// stop external interrupt
	immediateStopTriacTriggerDelay();
	sei();
	printf("triacrun stopped\n");
//	stopAmpsADC();
}

int16_t ampsADCValue()
{
	int16_t res;
	cli();
	res = lastAmpsADCVal;
	sei();
//	printf("ampsADC %i ",lastAmpsADCVal);
	return res;
}

/*
int16_t  valueFrom6Bit2Complement(int16_t adcV)
{
	if (adcV & 0x0200) {
		adcV--;
		adcV = ~(adcV | 0xFC00);
		adcV = - adcV;
	}
	return adcV;
}

int16_t diffADCValue()
{  
	int16_t res;
	res = ampsADCValue();
	res = valueFrom6Bit2Complement(res);
	return res;
}
*/


void stopAmpsADC()
{
	printf("stop ampsadc\n");
	cli();
	ADCSRA  = 0b00000111;  // disa ADC, ADATE, ADIE

	TCCR0B = 0b00000000  ; // stop timer 0
	TIMSK0  = 0b00000000;  //
	sei();
}


double adcVoltage()
{
	int16_t VHex;
	double   VFl;

	VFl = 0.0;

	VHex = ampsADCValue();
	VFl = (VHex * 5.0) / 0x03FF;
	
	return VFl;
}

void startDurationTimer(int16_t secs)
{
	durationTimerReachead = 0;
	secondsDurationTimerRemaining = secs;
	secondsInDurationTimer = 0;
	
	TIMSK1   = 0b00000010;  //  Output Compare A Match Interrupt Enable 
	TCCR1B = 0b00001101  ; // CTC on CC1A , set clk / 24, timer started 
}

void stopDurationTimer()
{
	TCCR1B = 0b00001000 ;  // CTC, timer stopped
	TIMSK1 = 0x00;
}
	


void setCompletionAlarmOff()
{
	PORTD &= ~0x08; 		
}

void setCompletionAlarmOn()
{
	PORTD |= 0x08; 	
}

void toggleCompletionAlarm()
{  
	if (PORTD && 0x08) {
		setCompletionAlarmOff();
	} else {
		setCompletionAlarmOn();
	}
}

