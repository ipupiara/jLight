
#if !defined(CCtateClassHeader)
#define CCtateClassHeader

#include "TStatechart.h"

TStatechart SJLightTriacStateChart;
TStatechart* PJLightTriacStateChart;



enum eEventTypes
{
	evTimeOutDurationTimer,
	evSecondsTick,
	evSec5Tick,
	evAdcTick,
	evProgrammingSwitchOn,
	evProgrammingSwitchOff,
	evRecordButtonOn,
	evRecordButtonOff,
	evTestButtonOn,
	evTestButtonOff,
	evStoreButtonPressed,
	evTimeoutRecord,
	evFatalError
};


typedef struct  {
	int evType;
	union {
		int8_t keyCode;
		struct {			// currently not in use
			float   voltage;  
			int8_t  potiPos;
			int8_t  jobType;
		} zeroAdjustingState;
	}  evData;
} CJLightEvent ;




void startStateCharts();


void stopStateCharts();


bool processTriacEvent(TStatechart* t,CJLightEvent* ev);




#endif


