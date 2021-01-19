/*
 * board.h
 *
 * Created: 01.12.2020 16:00:48
 *  Author: gertb
 */ 


#ifndef BOARD_H_
#define BOARD_H_

#define F_CPU	16000000UL
#define true	1
#define false	0


volatile uint8_t waiting = 0;

volatile int8_t rData[2] = {0, 0};
volatile uint16_t leds = 0;

// Inside sensor (0 - 300) -> too high for int8 (-128 to +127)! even for uint8 (0 .. 255)
// Outside sensor (0 - 300) -> see above
// Current pump speed (-100 - 100) -> not needed because raspberry already knows the values
// Current windmill speed (0 - 100) -> see above
// new settings:
// byte [0] Inside usonic MSB
// byte [1] Inside usonic LSB
// byte [2] Outside usonic MSB
// byte [3] outside usonic LSB
// byte [4] pump/gen speed offset 100 (0 => -100% --> 200 => 100%)
// byte [5] windmill speed in % (0 .. 100%)
volatile uint8_t tData[6] = {0, 0, 0, 0, 0, 0};


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

#define LED_GENERATOR	PH0
#define LED_PUMP		PH1

#define US_ECHO_IN		PD0
#define US_ECHO_OUT		PD1
#define US_TRIGGER_IN	PJ0
#define US_TRIGGER_OUT	PJ1

#define WM_PWM_OUT		PE3
#define GENERATOR_PWM	PB5
#define PUMP_PWM		PB6

#define SPI_SS_INT2		PD2

// LEDs
#define LED_SLOWEST_OCR	50000
#define LED_FASTEST_OCR  5000

//Pumps & Generators
#define PUMP_ICR			8000
#define PUMP_SLOWEST_OCR	4000

//Windmill
#define WM_ICR				8000
#define WM_SLOWEST_OCR		4000

#endif /* BOARD_H_ */