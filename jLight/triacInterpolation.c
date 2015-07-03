/*
 * triacInterpolation.c
 *
 * Created: 6/9/2015 10:03:22 AM
 *  Author: ixchel
 */ 

#include "string.h"
#include "triacInterpolation.h"
#include "TriacIntr.h"


int16_t getAndKeepCurrentValue()
{
	uint16_t  triacValue;  

	triacValue = getTriacFireDuration();
	pCurrentMinuteBuffer->data.buffer[currentPos] = triacValue;
	++pCurrentMinuteBuffer->data.amtEntries;
	return triacValue;
}

void retrieveStoredValue()
{
	uint16_t  triacValue;   // variable used for debugging purpose

	triacValue = pCurrentMinuteBuffer->data.buffer[currentPos];
	setTriacFireDuration(triacValue);
}



void resetInterpolation()
{
	pCurrentMinuteBuffer = &currentMinuteBuffer;
	currentSec = 0;
	currentSec10 = 0;
	currentPos = 0;
	timeoutRecord = 0;
}

void stepInterpolation()
{
	++ currentSec10;
	
	if(currentSec10 > 9)
	{
		++ currentSec;
		currentSec10 = 0;
		
	}
	
	if (currentRecMode == play) {
		if(currentPos >= currentMinuteBuffer.data.amtEntries) {
			resetInterpolation();
		} else {
			++ currentPos;	
		}
		retrieveStoredValue();
	} else if(currentRecMode == rec)  {
		if (currentPos <   maxDataAmt - endGapSize ) {   // tobe reviewed  ??
			getAndKeepCurrentValue();
			++ currentPos;
		}   else {
			timeoutRecord = 1;
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
	int16_t ampsADC = ampsADCValue();
	float ampsADCf = ampsADC;
	float pctf = ampsADCf / 1024.0;
	float triacFireDurationF = pctf * triggerDelayMax;
	int16_t triacFireDurationI = triacFireDurationF + 0.5;
	setTriacFireDuration(triacFireDurationI);
}

void setEndGap()
{
	int16_t  endDiff;
	int16_t cnt;
	float stepWidth;
	if ( (endDiff = pCurrentMinuteBuffer->data.buffer[currentPos] -  pCurrentMinuteBuffer->data.buffer[0]) >  5  ){
		stepWidth = endDiff / endGapSize ;
		for(cnt = 0; cnt < endGapSize; ++ cnt )  {
			pCurrentMinuteBuffer->data.buffer[currentPos] =  pCurrentMinuteBuffer->data.buffer[currentPos-1] + stepWidth;
			++currentPos;
		}
		pCurrentMinuteBuffer->data.buffer[currentPos] = pCurrentMinuteBuffer->data.buffer[0];
		++ currentPos;
	}
}

void clearBuffer()
{
	memset(pCurrentMinuteBuffer,0,sizeof(currentMinuteBuffer));
}