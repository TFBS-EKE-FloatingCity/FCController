/*
 * FCController.cpp
 *
 * Created: 01.12.2020 15:45:03
 * Author : gertb
 */ 

#include <avr/io.h>
#include "general.h"

#define DD_MISO    PB3

#define DD_MOSI    PB2

#define DD_SCK     PB1

#define DD_SS      PB0

#define DDR_SPI    DDRB

int8_t rData[2] = {0, 0};
	
	
// Inside sensor (0 - 300)
// Outside sensor (0 - 300)
// Current pump speed (-100 - 100)
// Current windmill speed (0 - 100)
int8_t tData[4] = {0, 0, 0, 0};

int main(void)
{
	
	//INIT Pump timer 1
	//PWM-frequency: 2kHz
	//>0% ... >40% Dutycycle
		
	
	//INIT windmill timer 3
	//PWM frequency: 25kHz
	//>0% ... >50% Dutycycle
	
	
	//INIT Usonictimer 4
	// 30cm max. depth of water => 60cm way of sound => ca. 2ms duration
	// 16Bit timer with prescaler 1 (16MHz clock) => after ca. 4ms Overflow => cancel meassurement and write 0 to array
	// 

	//INIT LED Timer 5
	// minValue for 1% (e.g.0.05s per LED)
	// maxValue for 100% (e.g. 0.5s per LED)
	// PWM Frequency >200Hz
	// Both directions with outputs A0 ... A15
	// two pins for MOSFETs to select the LED bank
	
	
	
	DDR_SPI |= (1<<DD_MISO);
	// Enable SPI
	SPCR |= (1<<SPE)|(1<<SPR0);
	
    while (1) 
    {
		// *** TESTING *** //
		tData[0] = 250;
		tData[1] = 175;
		tData[2] = -55;
		tData[3] = 50;
		// *** TESTING *** //		
		
		/************************************************************************/
		/*			   2 Bytes empfangen und in rData schreiben                 */
		/*						4 Bytes aus tData senden                        */
		/************************************************************************/
		// We will send 4 bytes so count up to 3
		for (uint8_t idx = 0; idx < 4; idx++) {
			
			// Write 1st byte into register
			SPDR = tData[idx];
			
			// Wait for transmission
			while(!(SPSR & (1<<SPIF)));
			
			// Because only the first 2 bytes are real data => check if its the 1st or 2nd byte
			if (idx < 2) {
				// Read register
				rData[idx] = SPDR;
			}
		}
		
		
		//TODO: Pumpe setzen
		//TODO: Windrad setzen
		//TODO: LED Lauflicht sezten
		//TODO: USonic Messung
		//TODO: für Test: die beiden rData Bytes in die ersten beiden tData Bytes schreiben
		
    }
}

