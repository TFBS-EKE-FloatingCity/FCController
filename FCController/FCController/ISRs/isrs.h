/*
 * isrs.h
 *
 * Created: 08.01.2021 16:42:47
 *  Author: gertb
 */ 


#ifndef ISRS_H_
#define ISRS_H_

#include<avr/interrupt.h>
#include "../general.h"

// stop Usonic measurement for it took too long
ISR(TIMER4_OVF_vect) {
	TCCR4B &= ~(1 << CS40);		// stop timer
	TCNT4 = 0;					// reset timer value
	waiting = 0;
}

// Usonic measurement (inside)
// gets called when the level on PD0 changes in every direction
// activate this interrupt only when distance measurement is active!
ISR(INT0_vect) {
	if(PIND & (1 << PD0)) {		// rising edge
		TCCR4B |= (1 << CS40);	// start timer
	} else {					// falling edge
		TCCR4B &= ~(1 << CS40);	// stop timer
		uint16_t dist = (uint16_t)((((double)((uint32_t)TCNT4 * 343UL) * 0.0625)/2000.0) + 0.5);
		tData[0] = (dist >> 8);
		tData[1] = dist;
		TCNT4 = 0;
		waiting = 0;
	}
}

// Usonic measurement (outside)
// gets called when the level on PD1 changes in every direction
// activate this interrupt only when distance measurement is active!
ISR(INT1_vect) {
	if(PIND & (1 << PD1)) {		// rising edge
		TCCR4B |= (1 << CS40);	// start timer
	} else {					// falling edge
		TCCR4B &= ~(1 << CS40);	// stop timer
		uint16_t dist = (uint16_t)((((double)((uint32_t)TCNT4 * 343UL) * 0.0625)/2000.0) + 0.5);
		tData[2] = (dist >> 8);
		tData[3] = dist;
		TCNT4 = 0;
	}
	waiting = 0;
}

// !SS falling edge 
// PIN PD2 must be connected to !SS pin
// stops the LED Timer because a transmission starts
ISR(INT2_vect) {
	TCCR5B &= ~((1 << CS50) | (1 << CS51));		// stop LED Timer
}

// LEDs
// when ICR is reached the next 16Bit LED value is set
ISR(TIMER5_CAPT_vect) {
	leds <<= 1;
	if(leds < 3) { 
		leds = 3;
	} else if (leds == 0x8000) {
		leds = 0x8001;
	} 
	PORTF = leds;
	PORTK = (leds >> 8);		
}


#endif /* ISRS_H_ */