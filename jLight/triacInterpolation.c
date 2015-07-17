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

uint8_t currentRecMode;

typedef struct StorageRecord  {
	int16_t val;
	int8_t  reps;
} StorageRecord ;
typedef  StorageRecord *  PStorageRecord;

int8_t*  nextRecP;
int8_t*  currentRecP;
int8_t*  endRecP;

int16_t	recordCounter;
int8_t	remainingReps;



int16_t valueOfRec(PStorageRecord pStoR)
{


	int16_t res = 0;
	res =  ( pStoR->val  &  ~0x8000);
	return res;	
}

void setValueOfRec(PStorageRecord pStoR, int16_t nVal)
{
	pStoR->val  &= 0x8000;
	pStoR->val  |= nVal;
}

int16_t  amtRepsOfRec(PStorageRecord pStoR)
{
	int16_t res = 0;
	if (pStoR->val & 0x8000)  {
		res = pStoR->reps;
	}
	return res;
}

void incRepsOfRec(PStorageRecord pStoR)
{
	if ( !(pStoR->val & 0x8000) ) {
		pStoR->val |= 0x8000;
		++nextRecP;
		pStoR->reps = 1;
	}  else {
		++pStoR->reps;
	}
}

int16_t unUsedMemory()
{
	int16_t res= 0;
	res = endRecP - nextRecP;
	return res;
}


int8_t storageBufferLow()
{
	int8_t res = 0;
	res = (unUsedMemory() < bufferLowSize);
	
	return res;
}

int8_t storageBufferCritical()
{
	int8_t res = 0;
	res = (unUsedMemory() < bufferCriticalSize);
	return res;
}

int8_t storageBufferTimeout()
{
	int8_t res = 0;
	res = (unUsedMemory() < bufferTimeoutSize);
	return res;
}

int8_t* nextRecOfRecInPlayFile(PStorageRecord curRP)
{
	int8_t* res = 0;
	if (curRP == NULL)  {
		res = (int8_t*) &currentStorageBuffer.data.buffer;
	}  else {
		 res = (int8_t*) curRP ;
		 res += 2;
		 if (amtRepsOfRec((PStorageRecord) curRP)>0) { res += 1;}
	}
	return res;
}


int16_t getNextValue()
{
	int16_t res = 0;
	if (remainingReps > 0) {
		--remainingReps;
	} else
	{
		if (recordCounter > pCurrentStorageBuffer->data.amtEntries) {
			resetInterpolation();
		}
		currentRecP = nextRecP;
		nextRecP = nextRecOfRecInPlayFile((PStorageRecord )currentRecP);
		remainingReps = amtRepsOfRec((PStorageRecord) currentRecP);
		++recordCounter;	
	}
	res = valueOfRec((PStorageRecord )currentRecP);
	return res;
}



int16_t keepValue(uint16_t  triacValue)
{
	int16_t d1Val;
	int16_t repsVal;
	if ((currentRecP == NULL)  || ( (d1Val = valueOfRec((PStorageRecord )currentRecP)) != triacValue ) 
			|| ( (repsVal = amtRepsOfRec((PStorageRecord )currentRecP)) > 250 )  )    { ////// args to go on next
		
		currentRecP = nextRecP;
		nextRecP += 2;
		++ recordCounter;
		++ pCurrentStorageBuffer->data.amtEntries;
		setValueOfRec((PStorageRecord)currentRecP,triacValue);
		
	} else {
		incRepsOfRec((PStorageRecord)currentRecP);
	}
	return triacValue;
}


int16_t setAndKeepCurrentValue()
{
	uint16_t  triacValue;  

	triacValue = getTriacFireDuration();
	keepValue(triacValue);
	return triacValue;
}



void retrieveNextStoredValue()
{
	uint16_t  triacValue;   // variable used for debugging purpose

	triacValue = getNextValue();
	setTriacFireDuration(triacValue);
}



void resetInterpolation()
{
	pCurrentStorageBuffer = &currentStorageBuffer;
	currentRecP = NULL;
	nextRecP  =  (int8_t *)   &currentStorageBuffer.data.buffer;
	endRecP  =  (int8_t *)   &currentStorageBuffer.data.endByte;
	recordCounter = 0;
	remainingReps = 0;
	timeoutRecord = 0;
}

void stepInterpolation()
{
	
	if (currentRecMode == play) {
		retrieveNextStoredValue();
	} else if(currentRecMode == rec)  {
		if ( !storageBufferTimeout() ) {   
			setAndKeepCurrentValue();
		}   else {
			timeoutRecord = 1;
		} 
	}	
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
	int16_t  firstValue = valueOfRec((PStorageRecord) &currentStorageBuffer.data.buffer);
	int16_t  lastValue  = valueOfRec((PStorageRecord) currentRecP);
	
	int16_t  endDiff  = lastValue -  firstValue;
	printf("setEndGap\n");
	if ( abs(endDiff) >  5  ){
		int16_t cnt;
		int16_t triacValue;
		float endDiffF = endDiff;
		float endGapSizeF = endGapSize;
		float stepWidth = endDiffF / endGapSizeF ;
		printf("endDiffF %11.3f  endGapSizeF %11.3f\n  stepWith %11.3f\n",endDiffF,endGapSizeF,stepWidth);    // line for testing and debugging only
		for(cnt = 0; cnt < endGapSize; ++ cnt )  {
			float triacValueF =    valueOfRec((PStorageRecord) currentRecP) - stepWidth;
			triacValue = triacValueF + 0.5;
			printf("setEndGap %i th value: %f11.3 -->  %i\n",cnt,triacValueF, triacValue);   // line for debugging and testing used only
			keepValue(triacValue);
		}
		triacValue = valueOfRec((PStorageRecord) &currentStorageBuffer.data.buffer);
		keepValue(triacValue);
	}
}

void clearBuffer()
{
	memset(pCurrentStorageBuffer,0,sizeof(currentStorageBuffer));
}
