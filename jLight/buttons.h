/*
 * buttons.h
 *
 * Created: 6/13/2015 4:57:43 PM
 *  Author: ixchel
 */ 


#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

enum { on,off
	
} lightState;

uint8_t  programmingSwitchOn;
uint8_t  programmingSwitchOff;
uint8_t  recordButtonOn;
uint8_t  recordButtonOff;
uint8_t  testButtonOn;
uint8_t  testButtonOff;
uint8_t  storeButtonPressed;

void initButtons();

void setRecordLight(int8_t toState);

void setTestLight(int8_t toState);

void setStoreLight(int8_t toState);

void setProgramingLight(int8_t toState);

#endif /* BUTTONS_H_ */