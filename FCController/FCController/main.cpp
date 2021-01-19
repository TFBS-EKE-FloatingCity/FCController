/*
 * FCController.cpp
 *
 * Created: 01.12.2020 15:45:03
 * Author : gertb
 */ 

#include <avr/io.h>
#include "general.h"
#include <util/delay.h>

int main(void)
{
	//INIT Watchdog
	//start Watchdog and set Watchdog LED (clear before entering while())
	DDRB |= (1 << WD_LED);
	PORTB |= ( 1 << WD_LED);		// set LED on
	wdt_reset();
	wdt_enable(WDTO_8S);			// start WDT TOF=8s
	
	//INIT Pump timer 1
	//PWM-frequency: 2kHz
	//>0% ... >40% Dutycycle
	DDRB |= (1 << PUMP_PWM) | (1 << GENERATOR_PWM);	// PWM pins as output
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12) |( 1 << WGM13);			// FastPWM mode 14
	ICR1 = PUMP_ICR -1;								// F_PWM = 2kHz at N=1 ==> 100% DutyCycle on OCR
	OCR1A = 0;										// DutyCyle = 0%
	OCR1B = 0;										// DutyCycle = 0%
		
	
	//INIT windmill timer 3
	//PWM frequency: 25kHz
	//>0% ... >50% Dutycycle
	DDRE |= (1 << WM_PWM_OUT);					// PWM pin as output
	TCCR3A |= (1 << WGM11);
	TCCR3B |= (1 << WGM12) | (1 << WGM13);		// FastPWM mode 14
	ICR3 = WM_ICR - 1;							// F_PWM = 2kHz at N=1 ==> 100% DutyCycle on OCR
	OCR3A = 0;									// DutyCycle = 0%
	
	//INIT 2x Usonic timer 4, INT0, INT1, 2xTrigger
	// 30cm max. depth of water => 60cm way of sound => ca. 2ms duration
	// 16Bit timer with prescaler 1 (16MHz clock) => after ca. 4ms Overflow => cancel meassurement and write 0 to array
	// not Timer output required, only set prescaler to start timer and configure Overflow Interrupt to end measurement. 
	// start Timer: TCCR4B |= (1 << CS40);
	// stopp Timer: TCCR4B &= ~(1 << CS40);
	TIMSK4 |= (1 << TOIE4);					// set interrupt (measurement takes too long)
	TCNT4 = 0;								// set counter to 0 
	EICRA |= (1 << ISC00) | (1 << ISC10);					// configure INT0 on PD0 and INT1 on PD1 to any edge for Sensor 1 and 2
	DDRJ |= (1 << US_TRIGGER_IN) | (1 << US_TRIGGER_OUT);	// configure PJ2 and PJ3 as Trigger output pins
	//EIMSK |= (1 << INT0) or  (1 << INT1);					// activate INT0 and INT1 ISR for Sensor 1 and 2
	


	//INIT LED Timer 5
	// minValue for 1% (e.g.0.05s per LED)
	// maxValue for 100% (e.g. 0.5s per LED)
	// PWM Frequency >200Hz
	// Both directions with outputs PF0 .. PF7 and PK0 .. PK7
	//      only plugged vice versa for other direction
	// two pins for MOSFETs to select the LED bank
	DDRF = 0xFF;
	DDRK = 0xFF;
	DDRH |= (1 << LED_GENERATOR) | (1 << LED_PUMP);		// Pins to select LED running light (per MOSFET)
	TCCR5B |= (1 << WGM52) |( 1 << WGM53);				// CTC mode 12
	ICR5 = LED_SLOWEST_OCR - 1;							// default value -> must be calculated
	TIMSK5 |= (1 << ICIE5);

	
	EICRA |= ( 1 << ISC21);							// enable INT2 on PD2 for falling edge (connect to !SS pin)
	
	TCCR1B |= (1 << CS10);				// start Timer 1 (Pumps) 
	TCCR3B |= (1 << CS10);				// start Timer 3 (Windmill) 
	sei();
	
	
	
	
	DDR_SPI |= (1<<DD_MISO);
	// Enable SPI
	SPCR |= (1<<SPE)|(1<<SPR0);
	PORTB &= ~(1 << WD_LED);			// clear init LED 
    while (1) 
    {
		wdt_reset();					// reset WDT => Counter = 0 
		//////////////////////////////////////////////////////////////////////////
		//                 calc pumps + LED Running Light
		//////////////////////////////////////////////////////////////////////////
		uint8_t absData = 0;
		if(rData[0] < 0) absData = rData[0] * (-1);
			else absData = rData[0];
		if(rData[0] == 0) {
			OCR1A = 0;
			PORTB &= ~((1 << PUMP_PWM) | (1 << GENERATOR_PWM));	
		} else {
			if(rData[0] > 0) {	// generator runs
				PORTB &= ~(1 << PUMP_PWM);		// disable pump leds mosfet
				PORTB |= (1 << GENERATOR_PWM);	// enable generator leds mosfet
			} else {
				PORTB &= ~(1 << GENERATOR_PWM);		// disable pump leds mosfet
				PORTB |= (1 << PUMP_PWM);			// enable generator leds mosfet
			}
			OCR1A = (uint16_t)((((uint32_t)absData * (uint32_t)(PUMP_ICR - PUMP_SLOWEST_OCR))/100) + PUMP_SLOWEST_OCR);
			ICR5 = (uint16_t)((((uint32_t)absData * (uint32_t)(LED_FASTEST_OCR - LED_SLOWEST_OCR))/100) + LED_SLOWEST_OCR);
		}
		
		//////////////////////////////////////////////////////////////////////////
		//                       calc Windmill - PWM
		//////////////////////////////////////////////////////////////////////////
		if(rData[1] <= 0) {
			OCR3A = 0;
		} else {
			OCR3A =  (uint16_t)((((uint32_t)rData[1] * (uint32_t)(WM_ICR - WM_SLOWEST_OCR))/100) + WM_SLOWEST_OCR);
		}
		
		//////////////////////////////////////////////////////////////////////////
		//                           USonic Messung
		//////////////////////////////////////////////////////////////////////////
		// Procedure for inner sensor: (same for outer sensor)
		// 1. set trigger for 20�s
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
		EIMSK |= (1 << INT2);					// enable !SS interupt when !SS is activated (falling edge)
		
		/************************************************************************/
		/*			   2 Bytes empfangen und in rData schreiben                 */
		/*						4 Bytes aus tData senden                        */
		/************************************************************************/
		// We will send 4 bytes so count up to 3
		for (uint8_t idx = 0; idx < 6; idx++) {
					
			// Write 1st byte into register
			SPDR = tData[idx];
					
			// Wait for transmission
			while(!(SPSR & (1<<SPIF)));
					
			// Because only the first 2 bytes are real data => check if its the 1st or 2nd byte
			if (idx < 2) {
				// Read register
				rData[idx] = SPDR;
				if (rData[idx] < 0) // to send values back next time transmitting
					tData[idx + 4] = (uint8_t)(rData[idx] + 100);	// transform to uint8 -> see declaration
				else
					tData[idx + 4] = (uint8_t)rData[idx];			// use positive value
			}
		}
		
    }
}

