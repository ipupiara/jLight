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

uint8_t  programmingSwitchOn;
uint8_t  programmingSwitchOff;
uint8_t  recordButtonOn;
uint8_t  recordButtonOff;
uint8_t  testButtonOn;
uint8_t  testButtonOff;
uint8_t  storeButtonPressed;

void initButtons();


#endif /* BUTTONS_H_ */