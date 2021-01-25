/*
 * general.h
 *
 * Created: 01.12.2020 15:59:34
 *  Author: gertb
 */ 


#ifndef GENERAL_H_
#define GENERAL_H_

#include "includes/board.h"
#include "includes/drivers.h"

// used in while to wait until usonic measurement is finished
 extern volatile uint8_t waiting;


// byte [0] is generator(>0) or pump (<0) in %
// byte [1] is windmill (>0) in %
extern volatile int8_t rData[2];	
extern volatile uint16_t leds;

// new settings:
// byte [0] Inside usonic MSB
// byte [1] Inside usonic LSB
// byte [2] Outside usonic MSB
// byte [3] outside usonic LSB
// byte [4] pump/gen speed offset 100 (0 => -100% --> 200 => 100%)
// byte [5] windmill speed in % (0 .. 100%)
extern volatile uint8_t tData[6];


#endif /* GENERAL_H_ */