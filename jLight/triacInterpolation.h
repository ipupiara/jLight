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

uint16_t currentSec;
uint8_t currentSec10;
minuteBuffer currentMinuteBuffer;
pMinuteBuffer pCurrentMinuteBuffer;


void calcInterpolation();

void resetInterpolation();

void stepInterpolation();


#endif /* TRIACINTERPOLATION_H_ */