/*
 * triacInterpolation.h
 *
 * Created: 6/9/2015 10:02:46 AM
 *  Author: ixchel
 */ 


#ifndef TRIACINTERPOLATION_H_
#define TRIACINTERPOLATION_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "TriacDefines.h"

#define maxDataAmt    180 * 5
#define endGapSize    3* 5

enum recMode  {
	rec,
	play
	};
void setRecMode(uint8_t recM);	
uint8_t currentRecMode;

uint16_t currentSec;
uint8_t currentSec10;

uint8_t timeoutRecord;


//void calcInterpolation();

void setEndGap();

void resetInterpolation();

void stepInterpolation();

void setTriacDelayByADC();

void clearBuffer();

#endif /* TRIACINTERPOLATION_H_ */