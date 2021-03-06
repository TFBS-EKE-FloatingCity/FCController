/*
 * FCController.cpp
 *
 * Created: 01.12.2020 15:45:03
 * Author : gertb
 */ 
#define F_CPU	16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "general.h"
#include "ISRs/isrs.h"


volatile uint8_t waiting;
volatile uint8_t rData[2];
volatile uint16_t leds;
volatile uint8_t tData[6];


int main(void)
{
	waiting = 0;
	rData[0] = 0;
	rData[1] = 0;
	leds = 0;
	
	for(uint8_t i = 0; i < 6; i++) {
		tData[i] = 0;
	}
	
	//Set LED Pins as output
	DDRB |= (1 << WD_LED) | (1 << LED_WindMill);
	// Turn on on-board LED (indicating start of Initialization phase)
	PORTB |= ( 1 << WD_LED);
	
	//INIT Watchdog with 8 seconds
	wdt_reset();
	wdt_enable(WDTO_8S);
	
	//------------------------------------------------------------------------------------------------//
	//INIT Pump timer 1
	//PWM-frequency: 2kHz
	//>0% ... >40% Dutycycle
    DDRB |= (1 << PUMP_PWM) | (1 << GENERATOR_PWM);	// PWM pins as output
	if(PUMP_PWM_ACTIVE)	//PWM Mode
	{
		TCCR1A |= (1 << WGM11);
		TCCR1B |= (1 << WGM12) |( 1 << WGM13);			// FastPWM mode 14
		TCCR1A |= (1 << COM1A1);						// set OC1A (PB5) as timer1 PWM output (generator)
		TCCR1A |= (1 << COM1B1);						// set OC1B (PB6) as timer 1 PWM output (pump)
		ICR1 = PUMP_ICR -1;								// F_PWM = 2kHz at N=1 ==> 100% DutyCycle on OCR
		OCR1A = 0;										// DutyCyle = 0%
		OCR1B = 0;										// DutyCycle = 0%
	}
	//------------------------------------------------------------------------------------------------//
		
	
	//------------------------------------------------------------------------------------------------//
	//INIT windmill timer 3
	//PWM frequency: 25kHz
	//>0% ... >50% Dutycycle
	DDRE |= (1 << WM_PWM_OUT);					// PWM pin as output
	TCCR3A |= (1 << WGM31);
	TCCR3B |= (1 << WGM32) | (1 << WGM33);		// FastPWM mode 14
	ICR3 = WM_ICR - 1;							// F_PWM = 2kHz at N=1 ==> 100% DutyCycle on OCR
	TCCR3A |= (1 << COM3A1);					// set OC3A (PE3) as timer 3 PWM output (windmill)
	OCR3A = 0;									// DutyCycle = 0%
	//------------------------------------------------------------------------------------------------//
	
	//------------------------------------------------------------------------------------------------//
	//INIT 2x USonic timer 4, INT0, INT1, 2xTrigger
	// 30cm max. depth of water => 60cm way of sound => 2ms duration
	// 16Bit timer with PreScaler 1 (16MHz clock) => after 4ms Overflow => cancel meassurement and write 0 to array
	// no Timer output required, only set PreScaler to start timer and configure Overflow Interrupt to end measurement. 
	// start Timer: TCCR4B |= (1 << CS40);
	// stopp Timer: TCCR4B &= ~(1 << CS40);
	TIMSK4 |= (1 << TOIE4);					// set interrupt (measurement takes too long)
	TCNT4 = 0;								// set counter to 0 
	EICRA |= (1 << ISC00) | (1 << ISC10);					// configure INT0 on PD0 and INT1 on PD1 to any edge for Sensor 1 and 2
	DDRJ |= (1 << US_TRIGGER_IN) | (1 << US_TRIGGER_OUT);	// configure PJ0 and PJ1 as Trigger output pins
	//EIMSK |= (1 << INT0) or  (1 << INT1);					// activate INT0 and INT1 ISR for Sensor 1 and 2
	//------------------------------------------------------------------------------------------------//
	

	//------------------------------------------------------------------------------------------------//
	//INIT LED Timer 5
	// minValue for 1% (e.g.0.05s per LED)
	// maxValue for 100% (e.g. 0.5s per LED)
	// PWM Frequency >320Hz
	// Both directions with outputs PF0 .. PF7 and PK0 .. PK7
	// only plugged vice versa for other direction
	// two pins for MOSFETs to select the LED bank
	DDRF = 0xFF;
	DDRK = 0xFF;
	DDRH |= (1 << LED_GENERATOR) | (1 << LED_PUMP);		// Pins to select LED running light (per MOSFET)
	TCCR5B |= (1 << WGM52) |( 1 << WGM53);				// CTC mode 12
	ICR5 = LED_SLOWEST_ICR - 1;							// default value -> must be calculated
	TIMSK5 |= (1 << ICIE5);
	
	EICRA |= ( 1 << ISC21);				// enable INT2 on PD2 for falling edge (connect to !SS pin)
	//------------------------------------------------------------------------------------------------//
	
	if(PUMP_PWM_ACTIVE)	//PWM Mode
	{
		TCCR1B |= (1 << CS10);				// start Timer 1 (Pumps) Prescaler 1
	}
	
	TCCR3B |= (1 << CS30);				// start Timer 3 (Windmill) Prescaler 1
	sei();								// enable interrupts global
	
	
	DDR_SPI |= (1<<DD_MISO);			// set MISO Pin to output
	
	SPCR |= (1<<SPE)|(1<<SPR0);			// Enable SPI
	
	
	// Turn off on-board LED (indicating end of Initialization phase)
	PORTB &= ~(1 << WD_LED);
	
    while (1) 
    {
		PORTB |= (1 << LED_WindMill);
		wdt_reset();					// Reset watchdog
		
		//////////////////////////////////////////////////////////////////////////
		//                 calculate pumps + LED Running Light
		//////////////////////////////////////////////////////////////////////////
			
		if(rData[0] == 0) {
			//OCR1A = 0;
			//OCR1B = 0;
			PORTH &= ~((1 << LED_PUMP) | (1 << LED_GENERATOR));	
		} else {
			if(rData[0] > 100) {
				PORTH |= (1 << LED_PUMP);		// enable pump leds MOSFET
				PORTH &= ~(1 << LED_GENERATOR);	// disable generator leds MOSFET
				
				if(PUMP_PWM_ACTIVE)
				{
					OCR1B = 0;
					OCR1A = (uint16_t)((((uint32_t)(rData[0] - 100) * (uint32_t)(PUMP_ICR - PUMP_FASTEST_OCR))/100) + PUMP_FASTEST_OCR);
					ICR5 = (uint16_t)((((uint32_t)(100-(rData[0] - 100)) * (uint32_t)(LED_FASTEST_ICR - LED_SLOWEST_ICR))/100) + LED_FASTEST_ICR);
				}
				else
				{
					PORTB |= (1 << PB5); //run generator
				}
				
				PORTB &= ~(1 << PB6); //stop pump
			} else {
				PORTH &= ~(1 << LED_PUMP);		// disable pump leds MOSFET
				PORTH |= (1 << LED_GENERATOR);	// enable generator leds MOSFET
				
				if(PUMP_PWM_ACTIVE)
				{
					OCR1A = 0;
					OCR1B = (uint16_t)((((uint32_t)(100 - rData[0]) * (uint32_t)(PUMP_ICR - PUMP_FASTEST_OCR))/100) + PUMP_FASTEST_OCR);
					ICR5 = (uint16_t)((((uint32_t)(rData[0]) * (uint32_t)(LED_FASTEST_ICR - LED_SLOWEST_ICR))/100) + LED_FASTEST_ICR);
				}
				else
				{
					PORTB |= (1 << PB6); //run pump
				}
				
				PORTB &= ~(1 << PB5); //stop generator
			}			
		}
		
		//////////////////////////////////////////////////////////////////////////
		//                       calculate Windmill - PWM
		//////////////////////////////////////////////////////////////////////////
		if(rData[1] == 0) {
			OCR3A = 0;
		} else {
			OCR3A =  (uint16_t)((((uint32_t)rData[1] * (uint32_t)(WM_ICR - WM_FASTEST_OCR))/100) + WM_FASTEST_OCR) - 1;
		}
		
		//////////////////////////////////////////////////////////////////////////
		//                           USonic Measuring
		//////////////////////////////////////////////////////////////////////////
		// Procedure for inner sensor: (same for outer sensor)
		// 1. set trigger for 20us
		// 2. activate extern interrupt sensing for INT0 and wait for change
		// 3. in ISR if rising edge start Timer 4
		// 4. wait for falling edge on INT0
		// 5. in ISR if falling edge stop Timer 4
		// 6. calculate distance and set tData 0 and 1
		// 7. stop INT0 sensing and set TCNT4 = 0
		PORTJ |= (1 << US_TRIGGER_IN);
		_delay_us(20);
		PORTJ &= ~(1 << US_TRIGGER_IN);
		EIMSK |= (1 << INT0);			// activate INT0 interrupt 
		waiting = 1;
		while(waiting){
		};
		EIMSK &= ~(1 << INT0);			// deactivate INT0 interrupt
		
		PORTJ |= (1 << US_TRIGGER_OUT);
		_delay_us(20);
		PORTJ &= ~(1 << US_TRIGGER_OUT);
		EIMSK |= (1 << INT1);			// activate INT1 interrupt 
		waiting = 1;
		while(waiting){
		};
		EIMSK &= ~(1 << INT1);			// deactivate INT1 interrupt 
		
		
		TCCR5B |= (1 << CS50) | (1 << CS51);	// start LED Timer	
		EIMSK |= (1 << INT2);					// enable !SS interrupt when !SS is activated (falling edge)
		PORTB &= ~(1 << LED_WindMill);			// turn out WindMill flash LED
		
		/************************************************************************/
		/*			   Receive 2 Bytes and Write them to rData                  */
		/*						Send 4 Bytes using tData                        */
		/************************************************************************/
				
		for (uint8_t idx = 0; idx < 6; idx++) {
					
			// Write 1st byte into register
			SPDR = tData[idx];
					
			// Wait for transmission
			while(!(SPSR & (1<<SPIF)));
					
			// Because only the first 2 bytes are real data => check if its the 1st or 2nd byte
			if (idx < 2) {
				
				rData[idx] = SPDR;				// SPI Read register
				tData[idx + 4] = rData[idx];	// to send values back next time transmitting
			}
		}
		
    }
}

