

#ifndef TriacIntr_H
	#define TriacIntr_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "TriacDefines.h"
	
	
int8_t durationTimerReachead;
int8_t runningSecondsTick;

int8_t adcTick;
int8_t sec5Tick;
uint8_t sec5Cnt;
uint8_t buttonSec10Dist;
void startAmpsADC();
void stopAmpsADC();
//void setDiffADC();
//void closeDiffADC();
void startSingleADC();
int16_t ampsADCValue();
int16_t diffADCValue();
double adcVoltage();

int16_t triacFireDurationCms;   // centi-millis-secs, not exactly but approximate, PID will handle the rest

void startDurationTimer(int16_t secs);
int16_t getSecondsDurationTimerRemaining();
int16_t getSecondsInDurationTimer();

void stopDurationTimer();

void initInterrupts();
void setTriacFireDuration(uint16_t cmsecs);
uint16_t getTriacFireDuration();

void startTriacRun();
void stopTriacRun();

void setCompletionAlarmOn();
void setCompletionAlarmOff();
void toggleCompletionAlarm();

/*
opto coupler based 0x (zerocross) handling. optocoupler based diode opens at 
*/

#endif
