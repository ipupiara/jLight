
#include <avr/io.h>
#include <stdio.h>
//#include <iostream.h>
//#include <assert.h>

#include "TStatechart.h"
#include "StateClass.h"
#include "TriacDefines.h"
#include "TriacIntr.h"
#include "triacInterpolation.h"


extern const uStInt uStIntHandlingDone;
extern const uStInt uStIntNoMatch;

CJLightEvent* currentEvent;

#include "TriacIntr.h"

// This defines and names the states the class has.
// attention: sequence must be the same as in xaStates (below)  !!!
enum eStates
{
	eStateJLightTriac,
	eStartState = eStateJLightTriac,
	eStateTriacOperating,
	eStateStartup,
	eStateTriacRunning,
	eStatePrograming,
	eStatePrepareForRec,
	eStateRecord,
	eStateRecInTime,
	eStateRecTimeLow,
	eStateRecTimeCritical,
	eStateRecTimeOut,  
	eStateTesting,
	eStateFatalError,   
	eNumberOfStates
};


uStInt evJLightChecker(void)
{
	return (uStIntNoMatch);
}

uStInt evTriacOperatingChecker(void)
{
	uStInt res = uStIntNoMatch;
//	printf("check for event in State evTriacOperating\n");

	if (currentEvent->evType == evFatalError) 
	{	
			BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStateFatalError);
				// No event action.
			END_EVENT_HANDLER(PJLightTriacStateChart);
			res =  uStIntHandlingDone;
	}
	
	return (res); 
}


void entryStartupState(void)
{
	printf("entry startup state\n");
//	displayCalibrationPrompt();
	startDurationTimer(6);

//	startDurationTimer(maxSecsPossible);
}

void exitStartupState(void)
{
	printf("exit ask calib\n");
	stopDurationTimer();
	//	clr_scr(); 
}

uStInt evStartupChecker(void)
{
	uStInt res = uStIntNoMatch;
//	printf("check for event in State evStateIdle\n");

	if (currentEvent->evType == evTimeOutDurationTimer) 
	{	
			BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStateTriacRunning);
				// No event action.
			END_EVENT_HANDLER(PJLightTriacStateChart);
			res =  uStIntHandlingDone;
	}
/*	if ((currentEvent->evType == evAstPressed) || (currentEvent->evType == evStartPressed))
	{	
			BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStateCalibrating);
				// No event action.
			END_EVENT_HANDLER(PJLightTriacStateChart);
			res =  uStIntHandlingDone;
	}
	if (currentEvent->evType == evF1Pressed) 
	{	
			BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStateChangeCalibVars);
				// No event action.
			END_EVENT_HANDLER(PJLightTriacStateChart);
			res =  uStIntHandlingDone;
	}
	if (currentEvent->evType == evSecondsTick) 
	{	
		displayCountDown();		
		res =  uStIntHandlingDone;
//		debugEvent1Triggered = 1;
	}   */
	return (res); 
}


void entryTriacRunningState(void)
{
		printf("entry eStateTriacRunning\n");
	resetInterpolation();
	setRecMode(play);
	startTriacRun();
}

void exitTriacRunningState(void)
{
	//	printf("exit FatalErro\n");
}

uStInt evTriacRunningChecker(void)
{
	uStInt res = uStIntNoMatch;
	//	printf("check for event in State evStateIdle\n");
	
	if (currentEvent->evType == evSec10Tick)
	{
		stepInterpolation();
//		displayCountDown();
		res =  uStIntHandlingDone;
		//		debugEvent1Triggered = 1;
	}
	if (currentEvent->evType == evProgrammingSwitchOn)
	{
		BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStatePrepareForRec);
		// No event action.
		END_EVENT_HANDLER(PJLightTriacStateChart);
		res =  uStIntHandlingDone;
	}
	return (res);
}



void entryProgramingState(void)
{
	//	printf("entry Programing\n");
	startAmpsADC();
}

void exitProgramingState(void)
{
	//	printf("exit Programing\n");
	stopAmpsADC();
}

uStInt evProgramingChecker(void)
{
	uStInt res = uStIntNoMatch;
	//	printf("check for event in State Programing\n");
	
	if (currentEvent->evType == evProgrammingSwitchOff)
	{
		BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStateTriacRunning);
		// No event action.
		END_EVENT_HANDLER(PJLightTriacStateChart);
		res =  uStIntHandlingDone;
	}
	if (currentEvent->evType == evStoreButtonPressed)
	{
//		BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStateTriacRunning);
		// No event action.
		
		syncStoreMinuteBuffer(pCurrentMinuteBuffer);
		
//		END_EVENT_HANDLER(PJLightTriacStateChart);
		res =  uStIntHandlingDone;
	}

	return (res);
}


void entryPrepareForRecState(void)
{
	//	printf("entry PrepareForRec\n");
}

void exitPrepareForRecState(void)
{
	//	printf("exit PrepareForRec\n");
}

uStInt evPrepareForRecChecker(void)
{
	uStInt res = uStIntNoMatch;
	//	printf("check for event in State PrepareForRec\n");
	
	if (currentEvent->evType == evAdcTick)
	{
		setTriacDelayByADC();
		//		displayCountDown();
		res =  uStIntHandlingDone;
		//		debugEvent1Triggered = 1;
	}
	if (currentEvent->evType == evRecordButtonOn)
	{
		BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStateRecord);
		// No event action.
		END_EVENT_HANDLER(PJLightTriacStateChart);
		res =  uStIntHandlingDone;
	}
	if (currentEvent->evType == evTestButtonOn)
	{
		BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStateTesting);
		// No event action.
		END_EVENT_HANDLER(PJLightTriacStateChart);
		res =  uStIntHandlingDone;
	}
	return (res);
}


void entryRecordState(void)
{
	//	printf("entry Record\n");
	clearBuffer();
	setRecMode(rec);
}

void exitRecordState(void)
{
	//	printf("exit Record\n");
	setRecMode(play);
}

uStInt evRecordChecker(void)
{
	uStInt res = uStIntNoMatch;
	//	printf("check for event in State Record\n");
	
	if (currentEvent->evType == evAdcTick)
	{
		setTriacDelayByADC();
		//		displayCountDown();
		res =  uStIntHandlingDone;
		//		debugEvent1Triggered = 1;
	}
	if (currentEvent->evType == evSec10Tick)
	{
		stepInterpolation();
		//		displayCountDown();
		res =  uStIntHandlingDone;
		//		debugEvent1Triggered = 1;
	}
	
	if (currentEvent->evType == evRecordButtonOff)
	{
		BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStatePrepareForRec);
		// No event action.
		END_EVENT_HANDLER(PJLightTriacStateChart);
		res =  uStIntHandlingDone;
	}
		
	if (currentEvent->evType == evTimeoutRecord)
	{
		BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStateRecTimeOut);
		// No event action.
		END_EVENT_HANDLER(PJLightTriacStateChart);
		res =  uStIntHandlingDone;
	}


	return (res);
}


void entryRecInTimeState(void)
{
	startDurationTimer(40);
}

void exitRecInTimeState(void)
{
	//	printf("exit RecInTime\n");
	stopDurationTimer();
}

uStInt evRecInTimeChecker(void)
{
	uStInt res = uStIntNoMatch;
	//	printf("check for event in State RecInTime\n");
	
	if (currentEvent->evType == evTimeOutDurationTimer)
	{
		BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStateRecTimeLow);
		// No event action.
		END_EVENT_HANDLER(PJLightTriacStateChart);
		res =  uStIntHandlingDone;
	}

	return (res);
}


void entryRecTimeLowState(void)
{
	//	printf("entry RecTimeLow\n");
	startDurationTimer(10);
}

void exitRecTimeLowState(void)
{
	//	printf("exit RecTimeLow\n");
	stopDurationTimer();
}

uStInt evRecTimeLowChecker(void)
{
	uStInt res = uStIntNoMatch;
	//	printf("check for event in State RecTimeLow\n");
	
	if (currentEvent->evType == evTimeOutDurationTimer)
	{
		BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStateRecTimeCritical);
		// No event action.
		END_EVENT_HANDLER(PJLightTriacStateChart);
		res =  uStIntHandlingDone;
	}


	return (res);
}


void entryRecTimeCriticalState(void)
{
	//	printf("entry RecTimeCritical\n");
}

void exitRecTimeCriticalState(void)
{
	//	printf("exit RecTimeCritical\n");
}

uStInt evRecTimeCriticalChecker(void)
{
	uStInt res = uStIntNoMatch;
	//	printf("check for event in State RecTimeCritical\n");
		if (currentEvent->evType == evTimeOutDurationTimer)
		{
			BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStateRecTimeLow);
			// No event action.
			END_EVENT_HANDLER(PJLightTriacStateChart);
			res =  uStIntHandlingDone;
		}


	return (res);
}


void entryRecTimeOutState(void)
{
	//	printf("entry RecTimeOut\n");
	startDurationTimer(4);
	setCompletionAlarmOn();
}

void exitRecTimeOutState(void)
{
	//	printf("exit RecTimeOut\n");
	stopDurationTimer();
	setCompletionAlarmOff();
}

uStInt evRecTimeOutChecker(void)
{
	uStInt res = uStIntNoMatch;
	//	printf("check for event in State RecTimeOut\n");
	
	if (currentEvent->evType == evTimeOutDurationTimer)
	{
		BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStatePrepareForRec);
		// No event action.
		END_EVENT_HANDLER(PJLightTriacStateChart);
		res =  uStIntHandlingDone;
	}

	return (res);
}


void entryTestingState(void)
{
	//	printf("entry Testing\n");
	resetInterpolation();
	setRecMode(play);
}

void exitTestingState(void)
{
	//	printf("exit Testing\n");
}

uStInt evTestingChecker(void)
{
	uStInt res = uStIntNoMatch;
	//	printf("check for event in State Testing\n");
	
	if (currentEvent->evType == evTestButtonOff)
	{
		BEGIN_EVENT_HANDLER(PJLightTriacStateChart, eStatePrepareForRec);
		// No event action.
		END_EVENT_HANDLER(PJLightTriacStateChart);
		res =  uStIntHandlingDone;
	}

	if (currentEvent->evType == evSec10Tick)
	{
		stepInterpolation();
		//		displayCountDown();
		res =  uStIntHandlingDone;
		//		debugEvent1Triggered = 1;
	}
	
	return (res);
}



void entryFatalErrorState(void)
{
//	printf("entry FatalError\n");
//	printf("**************fatal Error: %s *************************\n",lastFatalErrorString);
//	displayFatalError();
}

void exitFatalErrorState(void)
{
//	printf("exit FatalErro\n");
}

uStInt evFatalErrorChecker(void)
{
	uStInt res = uStIntNoMatch;
	//	printf("check for event in State evStateIdle\n");

	return (res);
}



#ifndef  sdccNULL 

#define tfNull 0

#else

t_fvoid  tfNull;

#endif 

// attention: sequence must be the same as above enum eStates

xStateType xaStates[eNumberOfStates] = {
 	{eStateJLightTriac,    // name
 	-1,									//parent
 	eStateTriacOperating,    // default substate
 	0,    // keep history
 	evJLightChecker,    // event checking fu
	tfNull,       // def state entry function
 	tfNull,     //    entering state
 	tfNull},     // exiting state

 	{eStateTriacOperating,
 		eStateJLightTriac,
 		eStateStartup,
 		0,									
 		evTriacOperatingChecker,
 		tfNull,
 		tfNull,
 	tfNull},

 	{eStateStartup,
 		eStateTriacOperating,
 		-1,
 		0,									
 		evStartupChecker,
 		tfNull,
 		entryStartupState,
 		exitStartupState
	},
	 
	{eStateTriacRunning,
		eStateTriacOperating,
		-1,
		0,
		evTriacRunningChecker,
		tfNull,
		entryTriacRunningState,
		exitTriacRunningState
	},

	{eStatePrograming,
		eStateTriacOperating ,
		eStatePrepareForRec,
		0,
		evProgramingChecker,
		tfNull,
		entryProgramingState,
		exitProgramingState
	},	 
	
	{eStatePrepareForRec,
		eStatePrograming ,
		-1,
		0,
		evPrepareForRecChecker,
		tfNull,
		entryPrepareForRecState,
		exitPrepareForRecState
	},
	
	{eStateRecord,
		eStatePrograming ,
		eStateRecInTime,
		0,
		evRecordChecker,
		tfNull,
		entryRecordState,
		exitRecordState
	},
	
	{eStateRecInTime,
		eStateRecord ,
		-1,
		0,
		evRecInTimeChecker,
		tfNull,
		entryRecInTimeState,
		exitRecInTimeState
	},

	{eStateRecTimeLow,
		eStateRecord ,
		-1,
		0,
		evRecTimeLowChecker,
		tfNull,
		entryRecTimeLowState,
		exitRecTimeLowState
	},
	
	{eStateRecTimeCritical,
		eStateRecord ,
		-1,
		0,
		evRecTimeCriticalChecker,
		tfNull,
		entryRecTimeCriticalState,
		exitRecTimeCriticalState
	},
	 
	{eStateRecTimeOut,
		eStatePrograming ,
		-1,
		0,
		evRecTimeOutChecker,
		tfNull,
		entryRecTimeOutState,
		exitRecTimeOutState
	},
	
	{eStateTesting,
		eStatePrograming ,
		-1,
		0,
		evTestingChecker,
		tfNull,
		entryTestingState,
		exitTestingState
	},
		

	 
	{eStateFatalError,
 		eStateJLightTriac,
 		-1,
 		0,
 		evFatalErrorChecker,
 		tfNull,
 		entryFatalErrorState,
 		exitFatalErrorState
	}	 	 	 
};



void startStateCharts()
{

#ifdef  sdccNULL 

	tfNull = (t_fvoid ) NULL;

#endif 

 	PJLightTriacStateChart = & SJLightTriacStateChart; 
	createTStatechart (& SJLightTriacStateChart, xaStates, eNumberOfStates, eStartState);
	
}


void stopStateCharts()
{
	destructTStatechart(&SJLightTriacStateChart);
}


bool processTriacEvent(TStatechart* t,CJLightEvent* ev)
{
	currentEvent = ev;
	return ProcessEvent(t);
}

