/*
 * triacInterpolation.c
 *
 * Created: 6/9/2015 10:03:22 AM
 *  Author: ixchel
 */ 

#include "triacInterpolation.h"
#include "TriacIntr.h"


int16_t getAndKeepCurrentValue()
{
	uint16_t  nextTriacValue;  

	nextTriacValue = getTriacFireDuration();
	pCurrentMinuteBuffer->data.buffer[currentPos] = nextTriacValue;
	++pCurrentMinuteBuffer->data.amtEntries;
	return nextTriacValue;
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
	currentPos = 0;
	getAndKeepCurrentValue();
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
				resetInterpolation();
			}
		}
		retrieveNextValue();
	} else if(currentRecMode == rec)
	{
		if (currentPos <   maxDataAmt - endGapSize ) {   // tobe reviewed  ??
			++ currentPos;
			getAndKeepCurrentValue();
		}   else{
			setEndGap();
		}
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

void setEndGap()
{
	int16_t  endDiff;
	int16_t cnt;
	int16_t amtSteps;
	float stepWidth;
	if ( (endDiff = pCurrentMinuteBuffer->data.buffer[currentPos] -  pCurrentMinuteBuffer->data.buffer[0]) >  5  ){
		stepWidth = endDiff / ( amtSteps = (endGapSize * 10));
		for(cnt = 0; cnt < amtSteps; ++ cnt )  {
			++ currentPos;
			pCurrentMinuteBuffer->data.buffer[currentPos] =  pCurrentMinuteBuffer->data.buffer[currentPos-1] + stepWidth;
		}
		pCurrentMinuteBuffer->data.buffer[currentPos] = pCurrentMinuteBuffer->data.buffer[0];
	}
}