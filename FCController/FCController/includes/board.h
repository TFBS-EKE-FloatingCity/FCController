/*
 * board.h
 *
 * Created: 01.12.2020 16:00:48
 *  Author: gertb
 */ 


#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#define F_CPU	16000000UL
#define true	1
#define false	0





//////////////////////////////////////////////////////////////////////////
//					SPI configurations									//
//////////////////////////////////////////////////////////////////////////
#define DD_MISO    PB3
#define DD_MOSI    PB2
#define DD_SCK     PB1
#define DD_SS      PB0
#define DDR_SPI    DDRB


//////////////////////////////////////////////////////////////////////////
//					Pin configurations									//
//////////////////////////////////////////////////////////////////////////

#define WD_LED			PB7
#define LED_GENERATOR	PH0		//MOSFET control for Generator LEDs
#define LED_PUMP		PH1		//MOSFET control for Pump LEDs
#define LED_WindMill	PB4

#define US_ECHO_IN		PD0
#define US_ECHO_OUT		PD1
#define US_TRIGGER_IN	PJ0
#define US_TRIGGER_OUT	PJ1

#define WM_PWM_OUT		PE3
#define GENERATOR_PWM	PB5
#define PUMP_PWM		PB6

#define SPI_SS_INT2		PD2

// LEDs
#define LED_SLOWEST_ICR	50000
#define LED_FASTEST_ICR  3000

//Pumps & Generators
#define PUMP_ICR			8000
#define PUMP_FASTEST_OCR	4000

//Windmill
#define WM_ICR				8000
#define WM_FASTEST_OCR		4000

#endif /* BOARD_H_ */