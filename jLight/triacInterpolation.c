/*
 * triacInterpolation.c
 *
 * Created: 6/9/2015 10:03:22 AM
 *  Author: ixchel
 */ 

#include "triacInterpolation.h"

void calcInterpolation()
{

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
	if(currentSec10 > 9)
	{
		++ currentSec;
		currentSec10 = 0;
		if(currentSec > currentMinuteBuffer.data.amtEntries) {
			currentSec = 0;
		}
	}
	calcInterpolation();
}