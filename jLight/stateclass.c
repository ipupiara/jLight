
#include <avr/io.h>
#include <stdio.h>
//#include <iostream.h>
//#include <assert.h>

#include "TStatechart.h"
#include "StateClass.h"


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
	eStateAskForCalibration,
/*	eStateChangeCalibVars,
	eStateCalibrating,
	eStateAskingRmsAvg,
	eStateCalibrateZeroSignal,
	eStateCalibrateScale,
	eStateCalibrateLow,
	eStateCalibrateHigh,
	eStateTriacIdle,
	eStateEditIdle,
	eStateEditAmps,
	eStateEditDuration,
	eStateSetup,
	eStateSetupIdle,
	eStateSetupAlarmYesNo,
	eStateSetupAlarmMinutes,
	eStateTriacRunning,
	eStateJobOkDisplay,   
*/	eStateFatalError,   
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

void entryAskForCalibrationState(void)
{
	printf("entry AskForCalibration\n");
//	displayCalibrationPrompt();
	startDurationTimer(6);

//	startDurationTimer(maxSecsPossible);
}

void exitAskForCalibrationState(void)
{
/*	printf("exit ask calib\n");
	stopDurationTimer();
	clr_scr(); */
}

uStInt evAskForCalibrationChecker(void)
{
	uStInt res = uStIntNoMatch;
//	printf("check for event in State evStateIdle\n");

	if (currentEvent->evType == evTimeOutDurationTimer) 
	{	
//			BEGIN_EVENT_HANDLER(PJoesTriacStateChart, eStateTriacIdle);
				// No event action.
//			END_EVENT_HANDLER(PJoesTriacStateChart);
//			res =  uStIntHandlingDone;
	}
/*	if ((currentEvent->evType == evAstPressed) || (currentEvent->evType == evStartPressed))
	{	
			BEGIN_EVENT_HANDLER(PJoesTriacStateChart, eStateCalibrating);
				// No event action.
			END_EVENT_HANDLER(PJoesTriacStateChart);
			res =  uStIntHandlingDone;
	}
	if (currentEvent->evType == evF1Pressed) 
	{	
			BEGIN_EVENT_HANDLER(PJoesTriacStateChart, eStateChangeCalibVars);
				// No event action.
			END_EVENT_HANDLER(PJoesTriacStateChart);
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
 	eStateAskForCalibration,
 	0,									
 	evTriacOperatingChecker,
 	tfNull,
 	tfNull,
 	tfNull},

 	{eStateAskForCalibration,
 	eStateTriacOperating,
 	-1,
 	0,									
 	evAskForCalibrationChecker,
 	tfNull,
 	entryAskForCalibrationState,
 	exitAskForCalibrationState},
	 
	{eStateFatalError,
 	eStateJLightTriac,
 	-1,
 	0,
 	evFatalErrorChecker,
 	tfNull,
 	entryFatalErrorState,
 	exitFatalErrorState}	 	 	 
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

