/*
 * FCController.cpp
 *
 * Created: 01.12.2020 15:45:03
 * Author : gertb
 */ 

#include <avr/io.h>
#include "general.h"
#include <util/delay.h>
#include "ISRs/isrs.h"


volatile uint8_t waiting;
volatile uint8_t rData[2];
volatile uint16_t leds;
volatile uint8_t tData[6];


int main(void)
{
	// initialize global variables 
	waiting = 0;
	rData[0] = 0;
	rData[1] = 0;
	leds = 0;
	
	for(uint8_t i = 0; i < 6; i++) {
		tData[i] = 0;
	}
	
	//INIT Watchdog
	//start Watchdog and set Watchdog LED (clear before entering while())
	DDRB |= (1 << WD_LED) | (1 << LED_WindMill);
	PORTB |= ( 1 << WD_LED);		// set LED on
	wdt_reset();
	wdt_enable(WDTO_8S);			// start WDT TOF=8s
	
	//INIT Pump timer 1
	//PWM-frequency: 2kHz
	//>0% ... >40% Dutycycle
	DDRB |= (1 << PUMP_PWM) | (1 << GENERATOR_PWM);	// PWM pins as output
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12) |( 1 << WGM13);			// FastPWM mode 14
	TCCR1A |= (1 << COM1A1);						// set OC1A (PB5) as timer1 pwm output (generator)
	TCCR1A |= (1 << COM1B1);						// set OC1B (PB6) as timer 1 pwm output (pump)
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
	TCCR3A |= (1 << COM3A1);					// set OC3A (PE3) as timer 3 pwm output (windmill)
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
	DDRJ |= (1 << US_TRIGGER_IN) | (1 << US_TRIGGER_OUT);	// configure PJ0 and PJ1 as Trigger output pins
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
	ICR5 = LED_SLOWEST_ICR - 1;							// default value -> must be calculated
	TIMSK5 |= (1 << ICIE5);
	
	EICRA |= ( 1 << ISC21);				// enable INT2 on PD2 for falling edge (connect to !SS pin)
	
	TCCR1B |= (1 << CS10);				// start Timer 1 (Pumps) Prescaler 1
	TCCR3B |= (1 << CS10);				// start Timer 3 (Windmill) Prescaler 1
	sei();								// enable interrupts global
	
	
	DDR_SPI |= (1<<DD_MISO);			// set MISO Pin to output
	
	SPCR |= (1<<SPE)|(1<<SPR0);			// Enable SPI
	PORTB &= ~(1 << WD_LED);			// clear init LED 
    while (1) 
    {
		PORTB |= (1 << LED_WindMill);
		wdt_reset();					// reset WDT => Counter = 0 
		//////////////////////////////////////////////////////////////////////////
		//                 calc pumps + LED Running Light
		//////////////////////////////////////////////////////////////////////////
			
		if(rData[0] == 0) {
			OCR1A = 0;
			PORTH &= ~((1 << LED_PUMP) | (1 << LED_GENERATOR));	
		} else {
			if(rData[0] > 100) {	// generator runs
				PORTH |= (1 << LED_PUMP);		// enable pump leds mosfet
				PORTH &= ~(1 << LED_GENERATOR);	// disable generator leds mosfet
				OCR1B = 0;
				OCR1A = (uint16_t)((((uint32_t)(rData[0] - 100) * (uint32_t)(PUMP_ICR - PUMP_FASTEST_OCR))/100) + PUMP_FASTEST_OCR);
				ICR5 = (uint16_t)((((uint32_t)(100-(rData[0] - 100)) * (uint32_t)(LED_FASTEST_ICR - LED_SLOWEST_ICR))/100) + LED_FASTEST_ICR);
			} else {
				PORTH &= ~(1 << LED_PUMP);		// disable pump leds mosfet
				PORTH |= (1 << LED_GENERATOR);	// enable generator leds mosfet
				OCR1A = 0;
				OCR1B = (uint16_t)((((uint32_t)(100 - rData[0]) * (uint32_t)(PUMP_ICR - PUMP_FASTEST_OCR))/100) + PUMP_FASTEST_OCR);
				ICR5 = (uint16_t)((((uint32_t)(rData[0]) * (uint32_t)(LED_FASTEST_ICR - LED_SLOWEST_ICR))/100) + LED_FASTEST_ICR);
			}			
		}
		
		//////////////////////////////////////////////////////////////////////////
		//                       calc Windmill - PWM
		//////////////////////////////////////////////////////////////////////////
		if(rData[1] <= 0) {
			OCR3A = 0;
		} else {
			OCR3A =  (uint16_t)((((uint32_t)rData[1] * (uint32_t)(WM_ICR - WM_FASTEST_OCR))/100) + WM_FASTEST_OCR) - 1;
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
		PORTB &= ~(1 << LED_WindMill);			// turn out WindMill flash LED
		
		//////////////////////////////////////////////////////////////////////////
		//							Test - delete when SPI connected
		//uint16_t test = 0;
		//test = (tData[2] << 8);
		//test |= (tData[3]);
		//rData[0] = (uint8_t)(test / 3);
		//if(rData[0]>100) rData[0] = 100;
		//rData[0] = 100 - rData[0];
		//_delay_ms(1000);
		//////////////////////////  end Test
		
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
				
				rData[idx] = SPDR;				// SPI Read register
				tData[idx + 4] = rData[idx];	// to send values back next time transmitting
			}
		}
		
    }
}

