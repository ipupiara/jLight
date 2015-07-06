/*
 * triacInterpolation.c
 *
 * Created: 6/9/2015 10:03:22 AM
 *  Author: ixchel
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "string.h"
#include "triacInterpolation.h"
#include "TriacIntr.h"


int16_t keepValue(uint16_t  triacValue)
{
	pCurrentMinuteBuffer->data.buffer[currentPos] = triacValue;
	++pCurrentMinuteBuffer->data.amtEntries;
	return triacValue;
}


int16_t setAndKeepCurrentValue()
{
	uint16_t  triacValue;  

	triacValue = getTriacFireDuration();
	keepValue(triacValue);
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
		if(currentPos >= currentMinuteBuffer.data.amtEntries - 1) {
			resetInterpolation();
		} else {
			++ currentPos;	
		}
		retrieveStoredValue();
	} else if(currentRecMode == rec)  {
		if (currentPos <   maxDataAmt - endGapSize ) {   
			setAndKeepCurrentValue();
			++ currentPos;
		}   else {
			timeoutRecord = 1;
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
	int16_t  endDiff  = pCurrentMinuteBuffer->data.buffer[currentPos-1] -  pCurrentMinuteBuffer->data.buffer[0];
	printf("setEndGap\n");
	if ( abs(endDiff) >  5  ){
		int16_t cnt;
		int16_t triacValue;
		float endDiffF = endDiff;
		float endGapSizeF = endGapSize;
		float stepWidth = endDiffF / endGapSizeF ;
		printf("endDiffF %11.3f  endGapSizeF %11.3f\n  stepWith %11.3f\n",endDiffF,endGapSizeF,stepWidth);    // line for testing and debugging only
		for(cnt = 0; cnt < endGapSize; ++ cnt )  {
			float triacValueF =  pCurrentMinuteBuffer->data.buffer[currentPos-1] - stepWidth;
			triacValue = triacValueF + 0.5;
			printf("setEndGap %i th value: %f11.3 -->  %i\n",cnt,triacValueF, triacValue);   // line for debugging and testing used only
			keepValue(triacValue);
			++currentPos;
		}
		triacValue = pCurrentMinuteBuffer->data.buffer[0];
		keepValue(triacValue);
	}
}

void clearBuffer()
{
	memset(pCurrentMinuteBuffer,0,sizeof(currentMinuteBuffer));
}