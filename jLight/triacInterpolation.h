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

enum recMode  {
	rec,
	play
	};
void setRecMode(uint8_t recM);	

uint8_t timeoutRecord;

void setEndGap();

void resetInterpolation();

void stepInterpolation();

void setTriacDelayByADC();

void clearBuffer();

int8_t storageBufferLow();

int8_t storageBufferCritical();

int16_t unUsedMemory();

#endif /* TRIACINTERPOLATION_H_ */