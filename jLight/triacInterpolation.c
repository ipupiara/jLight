/*
 * triacInterpolation.c
 *
 * Created: 6/9/2015 10:03:22 AM
 *  Author: ixchel
 */ 

#include "triacInterpolation.h"
#include "TriacIntr.h"


void calcInterpolation()
{
	uint16_t  nextTriacValue;   // variable used for debugging purpose
	if (currentRecMode == play) {
		nextTriacValue = pCurrentMinuteBuffer->data.buffer[currentPos];
		setTriacFireDuration(nextTriacValue);
	} else if (currentRecMode == rec) {
		nextTriacValue = getTriacFireDuration();
		pCurrentMinuteBuffer->data.buffer[currentPos] = nextTriacValue;
		++pCurrentMinuteBuffer->data.amtEntries;
	}
}

void keepCurrentValue()
{
	uint16_t  nextTriacValue;   // variable used for debugging purpose

	nextTriacValue = getTriacFireDuration();
	pCurrentMinuteBuffer->data.buffer[currentPos] = nextTriacValue;
	++pCurrentMinuteBuffer->data.amtEntries;
}

void retrieveNextValue()
{
	uint16_t  nextTriacValue;   // variable used for debugging purpose

	nextTriacValue = pCurrentMinuteBuffer->data.buffer[currentPos];
	setTriacFireDuration(nextTriacValue);
}

void resetInterpolation()
{
	pCurrentMinuteBuffer = &currentMinuteBuffer;
	currentSec = 0;
	currentSec10 = 0;
	calcInterpolation();
}

void stepInterpolation()
{
	++ currentSec10;
	++ currentPos;
	
	if (currentRecMode == play) {
		if(currentSec10 > 9)
		{
			++ currentSec;
			currentSec10 = 0;
			if(currentPos >= currentMinuteBuffer.data.amtEntries) {
				currentSec = 0;
				currentPos = 0;
			}
		}
		retrieveNextValue();
	} else if(currentRecMode == rec)
	{
		keepCurrentValue();
	}
//	calcInterpolation();
}

void setRecMode(uint8_t recM)
{
	currentRecMode = recM;
}

void setTriacDelayByADC()
{
	
}